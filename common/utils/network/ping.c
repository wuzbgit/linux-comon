#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/endian.h> /*define: __LITTLE_ENDIAN or __BIG_ENDIAN*/

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "types.h"
#include "debug/debug.h"
/*
#include <netinet/ip_icmp.h>

Don't include it for migration.
Just restructure: struct iphdr & struct icmphdr.
*/

// #undef   __DEBUG__
#define  __DEBUG__

/*copied from ./netinet/ip_icmp.h*/
#define ICMP_ECHOREPLY          0       /* Echo Reply                   */
#define ICMP_DEST_UNREACH       3       /* Destination Unreachable      */
#define ICMP_SOURCE_QUENCH      4       /* Source Quench                */
#define ICMP_REDIRECT           5       /* Redirect (change route)      */
#define ICMP_ECHO               8       /* Echo Request                 */
#define ICMP_TIME_EXCEEDED      11      /* Time Exceeded                */
#define ICMP_PARAMETERPROB      12      /* Parameter Problem            */
#define ICMP_TIMESTAMP          13      /* Timestamp Request            */
#define ICMP_TIMESTAMPREPLY     14      /* Timestamp Reply              */
#define ICMP_INFO_REQUEST       15      /* Information Request          */
#define ICMP_INFO_REPLY         16      /* Information Reply            */
#define ICMP_ADDRESS            17      /* Address Mask Request         */
#define ICMP_ADDRESSREPLY       18      /* Address Mask Reply           */

enum{
    IPVERSION   = 4,

    DEFAULT_LEN = 56,
    RAWBUF_SIZE = 128,
};

struct icmphdr {
    uint8_t  type;        /*message type*/
    uint8_t  code;	    /*message code*/

    uint16_t checksum;

    union{
        struct{
            uint16_t id;
            uint16_t sequence;
        }echo;

        uint32_t gateway;

        struct{
            uint16_t unsed;
            uint16_t mtu;
        }frag;              /*pmtu*/
    }un;

    uint8_t data[0];      /*ICMP data*/

#undef  icmp_id
#undef  icmp_seq
#define icmp_id     un.echo.id
#define icmp_seq    un.echo.sequence
};

struct iphdr {
    uint8_t ihl:4;
    uint8_t version:4;
    uint8_t   tos;
    uint16_t  tot_len;
    uint16_t  id;
    uint16_t  frag_off;
    uint8_t   ttl;            /*Time to Live*/
    uint8_t   protocol;
    uint16_t  checksum;
    uint32_t  saddr;
    uint32_t  daddr;
};

#define ICMP_HSIZE  sizeof(struct icmphdr)
#define IP_HSIZE    sizeof(struct iphdr)

static uint16_t checksum(uint8_t *buf, int32_t len)    
{    
    uint32_t    sum = 0;
    uint16_t * cbuf = (uint16_t *)buf;

    while(len > 1){
        sum += *cbuf++;
        len -= 2;
    }

    if(len){
        sum += *(uint8_t *)cbuf;
    }

    sum  = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return ~sum;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
static int32_t handle_recvkt(const int8_t * precvbuf, const struct sockaddr_in *from, uint16_t id, uint16_t seq)    
{
    const struct iphdr           *ip;
    const struct icmphdr         *icmp;

    int32_t                 ip_hlen;
    uint16_t              ip_datalen;

    ip          = (struct iphdr *)precvbuf;
    ip_hlen     = ip->ihl << 2;
    ip_datalen  = ntohs(ip->tot_len) - ip_hlen;
    
    icmp = (struct icmphdr *)(precvbuf + ip_hlen);
  
    if(checksum((uint8_t *)icmp, ip_datalen))  return -1;
    if(icmp->icmp_id  != id)                      return -1;
    if(icmp->icmp_seq != seq)                     return -1;
/*ICMP_ECHO: means echo the current & reply. 
 *ICMP_INFO_REQUEST: possibly is the best, especially for 127.0.0.1 loopbak.
 *However, ICMP_INFO_REQUEST is obsolete in some tcp/ip stack
 
    if(icmp->type     != ICMP_ECHOREPLY)         return -1;
 */

#if defined(__DEBUG__)
    {
        struct timeval      *psendtime;    
        struct timeval        recvtime;
        double                 rtt; /*round-trip time*/

        gettimeofday(&recvtime, NULL);
        
        psendtime = (struct timeval *)icmp->data; /*sent time*/

        rtt = ((&recvtime)->tv_sec - psendtime->tv_sec) * 1000 + ((&recvtime)->tv_usec - psendtime->tv_usec)/1000.0;  

        LOG_DEBUG("%d bytes from %s:icmp_seq=%u ttl=%d rtt=%.3f ms\n",
                ip_datalen,                 /*IP data length: icmp packet length*/
                inet_ntoa(from->sin_addr),  /*dst ip*/
                icmp->icmp_seq,             /*icmp sequence*/
                ip->ttl,                    /*live time*/
                rtt);                       /*round-trip time*/
    }
#endif

    return 0;    
}  

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
static int32_t ping_send(int32_t sockfd, const struct sockaddr_in *pdstaddr, uint16_t id, uint16_t seq)
{    
    struct iphdr        *ip_hdr;
    struct icmphdr      *icmp_hdr;
    int32_t           len, ret;

    int8_t sendbuf[RAWBUF_SIZE] = {0};

    /*ip header*/    
    ip_hdr = (struct iphdr *)sendbuf;
    
    ip_hdr->ihl      = sizeof(struct iphdr)>>2;
    ip_hdr->version  = IPVERSION;
    ip_hdr->tos      = 0;
    ip_hdr->tot_len  = IP_HSIZE + ICMP_HSIZE + DEFAULT_LEN; /*total length*/
    ip_hdr->id       = 0;
    ip_hdr->frag_off = 0;
    ip_hdr->protocol = IPPROTO_ICMP;/*ICMP protocol*/    
    ip_hdr->ttl      = 255;         /*the live time of a packet*/
    ip_hdr->daddr    = pdstaddr->sin_addr.s_addr;

    /*icmp header*/    
    icmp_hdr            = (struct icmphdr *)(sendbuf + (ip_hdr->ihl<<2));
    icmp_hdr->type      = ICMP_ECHO;    /*ICMP message type: ICMP_ECHO=8, ICMP_INFO_REQUEST=15*/
    icmp_hdr->code      = 0;
    icmp_hdr->icmp_id   = id;
    icmp_hdr->icmp_seq  = seq;

    memset(icmp_hdr->data, 0xff, DEFAULT_LEN);
    gettimeofday((struct timeval *)icmp_hdr->data,NULL); /* sending time */

    len=ip_hdr->tot_len;

    icmp_hdr->checksum  = 0; 
    icmp_hdr->checksum  = checksum((uint8_t *)icmp_hdr, len);

    ret = sendto(sockfd, sendbuf, len, MSG_DONTWAIT|MSG_NOSIGNAL, (struct sockaddr *)pdstaddr, sizeof(struct sockaddr_in));

#if defined(__DEBUG__)
    if(ret ==  len){
        LOG_DEBUG("Ping (%s): %d bytes data sent in ICMP packets.\n", inet_ntoa(pdstaddr->sin_addr), DEFAULT_LEN);
    }
#endif

    return (ret == len) ? 0 : -1;
}

/*
 *Return:
 *  0, if okay
 * -1, if failed
 */
static int32_t ping_recv(int32_t sockfd, uint16_t id, uint16_t seq, int32_t timeout/*millisecond*/)
{
    socklen_t   len;
    int32_t  ret, errno;

    struct timeval tm;
    struct sockaddr_in from;

    int8_t recvbuf[RAWBUF_SIZE] = {0};

    if(timeout > 0){
        tm.tv_sec  =  timeout/1000;
        tm.tv_usec = ((uint64_t)timeout-((uint64_t)timeout/1000)*1000)*1000;

        ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(struct timeval));

        if (ret < 0) return -1;
    }

    len = sizeof(from);

    do{
        errno = 0;

        ret = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &len);

        if(ret <= 0){
            if(EINTR == errno){
                continue;
            }
        }

        break;
    }while(1);

    if(ret <= 0) ret = -1;
    else{
        ret = handle_recvkt(recvbuf, &from, id, seq);
    }

    return ret;
}    

/*
 *Return:
 * >0, if okay, return the successful times
 * -1, if failed
 */
int32_t ping(const int8_t *daddr, uint16_t id, int32_t times,uint32_t timeout)
{
    struct in_addr         inp;
    struct sockaddr_in    dest;
    int32_t          sockfd = 0, ret = 0, on = 1, itimes = 0, success = 0;

    if((NULL == daddr) || (times <= 0)) return -1;

    if (timeout == 0) {
        timeout = 100;
    }
    /*
     * setuid(getuid());
     */

    ret = inet_pton(AF_INET, daddr, &inp);
    if(ret <= 0){
        ret = -1;
        LOG_ERROR("ping: destinaiton ip address is wrong on id %d\n",id);
        goto error_;
    }

    memset(&dest,0,sizeof dest);   
    dest.sin_family = PF_INET;        /*PF_INET is IPV4*/     
    dest.sin_port   = ntohs(0);
    dest.sin_addr   = inp;

    sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0){
        ret = -1;
        LOG_ERROR("ping: RAW socket created error on id %d\n",id); 
        goto error_;
    }    
  
    /*IP_HDRINCL: the caller fills IP header by itself*/
    on = 1;
    ret = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));      
    if(ret < 0){
        ret = -1;
        LOG_ERROR("ping: setsockopt: IP_HDRINCL failed on id %d\n",id);
        goto error_;
    }  

    itimes = success = 0;
    do{
        ret = ping_send(sockfd, &dest, id, itimes);

        if(0 == ret){
            ret = ping_recv(sockfd, id, itimes, timeout);

            if(0 == ret) success++;
        }
        else {
            LOG_ERROR("ping %s %d failed errno=%d (%s)\n",daddr,id,errno,strerror(errno));
        }
    }while(++itimes < times);

    ret = (success > 0)? success : -1;

error_:
    if(sockfd >= 0) close(sockfd);

    return ret;
}

/*
 *Return:
 * >0, if okay, return the successful times
 * -1, if failed
 */
int32_t ping_intf(const int8_t *daddr, const char* intf, uint16_t id, int32_t times,uint32_t timeout)
{
    struct in_addr         inp;
    struct sockaddr_in    dest;
    int32_t          sockfd = 0, ret = 0, on = 1, itimes = 0, success = 0;
    int interface_index;

    if((NULL == daddr) || (times <= 0)) return -1;

    interface_index = if_nametoindex(intf);
    if (interface_index == 0) {
        perror("if_nametoindex failed");
        return -1;
    }

    if (timeout == 0) {
        timeout = 100;
    }
    /*
     * setuid(getuid());
     */

    ret = inet_pton(AF_INET, daddr, &inp);
    if(ret <= 0){
        ret = -1;
        LOG_ERROR("ping: destinaiton ip address is wrong on id %d\n",id);
        goto error_;
    }

    memset(&dest,0,sizeof dest);   
    dest.sin_family = PF_INET;        /*PF_INET is IPV4*/     
    dest.sin_port   = ntohs(0);
    dest.sin_addr   = inp;

    sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0){
        ret = -1;
        LOG_ERROR("ping: RAW socket created error on id %d\n",id); 
        goto error_;
    }    
  
    /*IP_HDRINCL: the caller fills IP header by itself*/
    on = 1;
    ret = setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));      
    if(ret < 0){
        ret = -1;
        LOG_ERROR("ping: setsockopt: IP_HDRINCL failed on id %d\n",id);
        goto error_;
    }  

    int rc = setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, &interface_index, sizeof(interface_index));
    if (rc < 0) {
        perror("setsockopt failed");
        close(sockfd);
        goto error_;
    }

    itimes = success = 0;
    do{
        ret = ping_send(sockfd, &dest, id, itimes);

        if(0 == ret){
            ret = ping_recv(sockfd, id, itimes, timeout);

            if(0 == ret) success++;
        }
        else {
            LOG_ERROR("ping %s %d failed errno=%d (%s)\n",daddr,id,errno,strerror(errno));
        }
    }while(++itimes < times);

    ret = (success > 0)? success : -1;

error_:
    if(sockfd >= 0) close(sockfd);

    return ret;
}
