#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <net/route.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <fcntl.h>
#include <pthread.h>
#include "types.h"
#include "netip.h"

#include "debug/debug.h"

/*
 *Used for is_valid_ip2() checking
 */
#define DEFAULT_ETHIF_FORCHECK "eth0:5"

static int get_addr(const char *netif, int flag, char *o_addr, int size)
{
    int sockfd = 0;
    struct sockaddr_in *sin;
    struct ifreq ifr;
    unsigned char hwaddr[6];
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG_ERROR("socket error!\n");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    if (snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", netif) <= 0)
    {
        LOG_ERROR("Cannot assign ifr name\n");
        close(sockfd);
        return -1;
    }

    if(ioctl(sockfd, flag, &ifr) < 0 )
    {
        LOG_ERROR("ioctl error!\n");
        close(sockfd);
        return -1;
    }
    
    close(sockfd);

    if (SIOCGIFHWADDR == flag){
        memcpy((void *)hwaddr, (const void *)&ifr.ifr_ifru.ifru_hwaddr.sa_data, 6);
        if (snprintf(o_addr, size, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", hwaddr[0],hwaddr[1],hwaddr[2],hwaddr[3],hwaddr[4],hwaddr[5]) <= 0)
        {
            LOG_ERROR("set o_addr error!\n");
            return -1;
        }
    }else{
        sin = (struct sockaddr_in *)&ifr.ifr_addr;
        if (snprintf((char *)o_addr, size, "%s", inet_ntoa(sin->sin_addr)) <= 0)
        {
            LOG_ERROR("set o_addr error!\n");
            return -1;
        }
    }

    return 0;
}

static int set_addr(const char *netif, const char addr[16], int flag)
{
    struct ifreq ifr;
    struct sockaddr_in sin;
    int sockfd = -1, res = -1;
    int lerrno =  0;

    if (!is_valid_ip(addr)){
        LOG_ERROR("addr was invalid!\n");
        goto exit_;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd <= 0){
        LOG_ERROR("Could not get socket.\n");
        goto exit_;
    }

    if (snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", netif) <= 0)
    {
        LOG_ERROR("Cannot assign ifr name\n");
        goto exit_;
    }

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        LOG_ERROR("Cannot read ipflags\n");
        goto exit_;
    }

    memset(&sin, 0, sizeof(struct sockaddr));

    sin.sin_family = AF_INET;

    res = inet_pton(AF_INET, addr, &sin.sin_addr);
    if(res <= 0){
        res = -1;

        lerrno = errno;
        LOG_ERROR("Cannot get IP address on error %d,(%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

    if (ioctl(sockfd, flag, &ifr) < 0){
        lerrno = errno;
        LOG_ERROR("Cannot set IP address on error %d,(%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    usleep(1000*100);

    ifr.ifr_flags |= IFF_UP |IFF_RUNNING;     
    if( ioctl( sockfd, SIOCSIFFLAGS, &ifr ) < 0 )   
    {
        lerrno = errno;
        LOG_ERROR("Cannot set ifflags on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    usleep(1000*100);

    res = 0;

exit_:
    if(sockfd >= 0){
        close(sockfd);
    }

    return res;
}

static int set_ip2netmask(const char *netif, const char addr[16], const char netmask[16])
{
    struct ifreq ifr;
    struct sockaddr_in sin;
    int sockfd = -1, res = -1;
    int lerrno =  0;

    if (!is_valid_ip(addr)){
        LOG_ERROR("ip's addr was invalid!\n");
        goto exit_;
    }

    if (!is_valid_netmask(netmask, 0)){
        LOG_ERROR("ip's netmask was invalid!\n");
        goto exit_;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd <= 0){
        LOG_ERROR("Could not get socket on setting ip & netmask.\n");
        goto exit_;
    }

    if (snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", netif) <= 0)
    {
        LOG_ERROR("Cannot assign ifr name\n");
        goto exit_;
    }

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        LOG_ERROR("Cannot read SIOCGIFFLAGS to set ip & netmask\n");
        goto exit_;
    }

    memset(&sin, 0, sizeof(struct sockaddr));

    sin.sin_family = AF_INET;

    /* set ipaddr*/
    res = inet_pton(AF_INET, addr, &sin.sin_addr);
    if(res <= 0){
        res = -1;

        lerrno = errno;
        LOG_ERROR("Cannot get one ip's address on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

    if (ioctl(sockfd, SIOCSIFADDR, &ifr) < 0){
        lerrno = errno;
        LOG_ERROR("Cannot set one ip's address on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    /* set netmask*/
    res = inet_pton(AF_INET, netmask, &sin.sin_addr);
    if(res <= 0){
        res = -1;

        lerrno = errno;
        LOG_ERROR("Cannot get ip's netmask on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

    if (ioctl(sockfd, SIOCSIFNETMASK, &ifr) < 0){
        lerrno = errno;
        LOG_ERROR("Cannot set ip's netmask on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    ifr.ifr_flags |= IFF_UP |IFF_RUNNING;     
    if( ioctl( sockfd, SIOCSIFFLAGS, &ifr ) < 0 )   
    {
        lerrno = errno;
        LOG_ERROR("Cannot set ifflags on error %d (%s)\n",lerrno,strerror(lerrno));
        goto exit_;
    }

    usleep(1000*100);

    res = 0;

exit_:
    if(sockfd >= 0){
        close(sockfd);
    }

    return res;
}

static int down_netif(const char *netif)
{
    struct ifreq ifr;
    int sockfd = -1, res = -1;
    int lerrno =  0;

    if((NULL == netif) || ('\0' == netif[0])) return -1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd <= 0){
        LOG_ERROR("Could not get socket.\n");
        goto exit_;
    }

    strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        LOG_ERROR("Cannot read ipflags\n");
        goto exit_;
    }

    strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));

    ifr.ifr_flags &= ~(IFF_UP |IFF_RUNNING);
    if( ioctl( sockfd, SIOCSIFFLAGS, &ifr ) < 0 )   
    {
        lerrno = errno;

        if(EADDRNOTAVAIL != lerrno){
            LOG_ERROR("Cannot set ifflags on error %d (%s)\n",lerrno,strerror(lerrno));
        }

        goto exit_;
    }

    usleep(1000*100);

    res = 0;

exit_:
    if(sockfd >= 0){
        close(sockfd);
    }

    return res;
}

static int up_netif(const char *netif)
{
    struct ifreq ifr;
    int sockfd = -1, res = -1;
    int lerrno =  0;

    if((NULL == netif) || ('\0' == netif[0])) return -1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd <= 0){
        LOG_ERROR("Could not get socket.\n");
        goto exit_;
    }

    strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));

    /* Read interface flags */
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
        LOG_ERROR("Cannot read ipflags\n");
        goto exit_;
    }

    strncpy(ifr.ifr_name, netif, sizeof(ifr.ifr_name));

    ifr.ifr_flags |= IFF_UP |IFF_RUNNING;
    if( ioctl( sockfd, SIOCSIFFLAGS, &ifr ) < 0 )
    {
        lerrno = errno;

        if(EADDRNOTAVAIL != lerrno){
            LOG_ERROR("Cannot set ifflags on error %d (%s)\n",lerrno,strerror(lerrno));
        }

        goto exit_;
    }

    usleep(1000*100);

    res = 0;

exit_:
    if(sockfd >= 0){
        close(sockfd);
    }

    return res;
}


/*
 *Return:
 * >0, if okay, the character size in <o_buffer> without '\0'
 * -1, if failed
 */
static int32_t help_ipconverter(const int8_t *i_buffer, int32_t i_size, int8_t *o_buffer, int32_t o_maxsize)
{
    int32_t ret = 0, res = -1, num1 = 0, num2 = 0, num3 = 0, num4 = 0;

    ret = sscanf(i_buffer, "%d.%d.%d.%d", &num1, &num2, &num3, &num4);

    if(4 == ret){
        ret = snprintf(o_buffer, o_maxsize, "%d.%d.%d.%d", num1, num2, num3, num4);

        if(ret > 0 && ret < o_maxsize){
            res = ret;
        }
    }

    return res;
}

bool is_valid_ip(const char ipaddr[16])
{
    int        ret = 0;
    struct in_addr inp;

    static
    struct in_addr inp_printlimited;

    if((NULL == ipaddr) || ('\0' == ipaddr[0])) return false;

    ret = inet_pton(AF_INET, ipaddr, &inp);
    if (ret <= 0){
        LOG_WARN("warning: inet_pton:ip=%s\n",ipaddr);
        return false;
    }

    if(inp_printlimited.s_addr != inp.s_addr){
        inp_printlimited = inp;

        LOG_INFO("info: inet_pton:ip=%s",inet_ntoa(inp));
    }

    return true;
}

/*
 *SIOCDIFADDR is unavailable in linux
 *
 *To check whether IP is valid, we have to depend on NETIF
 *
 *Notice:
 *  This IFC is just used by IP, not a netmask, nor subnet
 */
bool is_valid_ip2(const char ipaddr[16])
{
    int32_t ret = 0;

    if(!is_valid_ip(ipaddr)) return false;

    ret = set_localip(DEFAULT_ETHIF_FORCHECK, ipaddr);
    if(0 == ret){
        ret = down_netif(DEFAULT_ETHIF_FORCHECK);
    }
    else if(17/*File exists*/ == errno){
        ret = 0;
    }

    return (0 == ret) ? true : false;
}

/*
 * 先验证是否为合法IP，然后将掩码转化成32无符号整型，取反为000...00111...1，
 * 然后再加1为00...01000...0，此时为2^n，如果满足就为合法掩码
 * 
 * <expected>: 
 *      0, means NO expected; 
 *  Not 0, means expecting the netmask should be that. Totally depends on the caller.
 *
 */
bool is_valid_netmask(const char netmask[16], uint32_t expected)
{
    uint32_t b   = 0, bn = 0, i = 0, n[4] = {0};
    int32_t ret = 0;

    if(is_valid_ip(netmask)){
        ret = sscanf(netmask, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);

        if(4 == ret){
            /*convert into 32bits integer*/
            for(i = 0; i < 4; ++i){
                b += n[i] << (i * 8);
            }

            /*check if it is 2^n */
            bn = b;
            b = ~b + 1;

            if((b & (b - 1)) == 0){
                if     (0 == expected)  return true;
                else if(bn == expected)  return true;
            }
        }
    }

    return false;
}

/* 
 *<expected>: 
 *      0, means NO expected; 
 *  Not 0, means expecting the netmask should be that. Totally depends on the caller.
 *
 */
bool is_valid_netmask2(uint32_t netmask, uint32_t expected)
{
    uint32_t b   = netmask;

    /*check if it is 2^n */
    b = ~b + 1;

    if((b & (b - 1)) == 0){
        if     (0 == expected)  return true;
        else if(netmask == expected)  return true;
    }

    return false;
}

int get_localip(const char *netif, char *o_ipaddr, int size)
{
    return get_addr(netif, SIOCGIFADDR, o_ipaddr, size);
}

int get_localnetmask(const char *netif, char *o_netmask, int size)
{
    return get_addr(netif, SIOCGIFNETMASK, o_netmask, size);
}

int get_localmac(const char *netif, char * o_hwaddr, int size)
{
    return get_addr(netif, SIOCGIFHWADDR, o_hwaddr, size);
}

int set_localnetmask(const char *netif, const char *szNetMask)
{
    return set_addr(netif, szNetMask, SIOCSIFNETMASK);
}

int set_localip(const char *netif, const char *ipaddr)
{
    return set_addr(netif, ipaddr, SIOCSIFADDR);
}

int set_localip2netmask(const char *netif, const char *ipaddr, const char *szNetMask)
{
    return set_ip2netmask(netif, ipaddr, szNetMask);
}

int set_localgateway(const char *gw)
{
    int sockFd;
    struct sockaddr_in sockaddr;
    struct rtentry rt;

    if (!is_valid_ip(gw))
    {
        LOG_ERROR("gateway was invalid!\n");
        return -1;
    }

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0)
    {
        LOG_ERROR("Socket create error.");
        return -1;
    }

    memset(&rt, 0, sizeof(struct rtentry));
    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = 0;

    if(inet_pton(AF_INET, gw, &sockaddr.sin_addr) <= 0)
    {
        LOG_ERROR("inet_pton error\n");
        close(sockFd);
        return -1;
    }

    memcpy ( &rt.rt_gateway, &sockaddr, sizeof(struct sockaddr_in));
    ((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
    ((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
    rt.rt_flags = RTF_GATEWAY;
    if (ioctl(sockFd, SIOCADDRT, &rt)<0)
    {
        int lerrno = errno;

        close(sockFd);

        if(17 != lerrno){
            LOG_ERROR("ioctl(SIOCADDRT) error in set_default_route, errno=%d (%s)\n",lerrno,strerror(lerrno));
        }

        return (17 == lerrno) ? 0: -1;
    }

    close(sockFd);

    return 0;
}

int get_localgateway(const char *netif, char *o_gw, int size)
{   
    FILE *fp;   
    char buf[256]  = {0}; // 128 is enough for linux   
    char iface[16] = {0};   
    unsigned long dest_addr, gate_addr;   
    int ret = 0;

    struct in_addr inp = {INADDR_NONE};

    fp = fopen("/proc/net/route", "r");   
    if (fp == NULL)   
        return -1;   

    /* Skip title line */   
    fgets(buf, sizeof(buf), fp);
    while (fgets(buf, sizeof(buf), fp)) {
        if (sscanf(buf, "%s\t%lX\t%lX", iface, &dest_addr, &gate_addr) != 3 || dest_addr != 0 || gate_addr == 0)
            continue;

        if(0 != strncmp(iface, netif, strlen(iface)))
            continue;

        inp.s_addr = gate_addr;
        break;
    }

    fclose(fp);
    if(INADDR_NONE != inp.s_addr){
        ret = snprintf(o_gw, size, "%s", inet_ntoa(inp));
    }

    return ret > 0 ? 0 : -1;
}  

int down_localnetif(const char *netif)
{
    return down_netif(netif);
}

int up_localnetif(const char *netif)
{
    return up_netif(netif);
}

bool netif_exist(const char *ifc)
{
    int sockfd;
    struct ifreq ifr;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Failed to create socket");
        exit(-1);
    }
    
    strncpy(ifr.ifr_name, ifc, IFNAMSIZ-1);   
    
    // 发送SIOCGIFFLAGS命令到内核，获取接口状态标志位
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) != 0) {
        close(sockfd);
        return false;
    }
        
    close(sockfd);
    
    return true;
}
 
bool net_check_default_route(const char *ifc)
{
    FILE *file;
    char buffer[1024];
    char fields[9][128];
    bool exist = false;
    char *ptr = NULL;
    char *ptr_src = NULL;
    file = fopen("/proc/net/route", "r");
    if (file == NULL) {
        perror("Failed to open /proc/net/route");
        
        return false;
    }

    while (1) {
        memset(buffer,0,sizeof(buffer));
        memset(fields,0,sizeof(fields));
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            break;
        }
        // Skip the first line containing headers
        if (!strncmp(buffer, "Iface\tDestination\tGateway\tFlags\tRefCnt\tUse\tMetric\tMask\tMTU\tWindow\tIRTT", strlen("Iface"))) {
            continue;
        }
        
        int i = 0;
        ptr_src = buffer;
        ptr = strtok(buffer, "\t");
        if (ptr == NULL) {
            continue;
        }
        strncpy(fields[i],ptr_src,ptr-ptr_src);
        ptr_src = ptr;
        i++;
        
        while (i <= 8) {
            ptr = strtok(NULL, "\t");
            if (ptr == NULL) {
                break;
            }
            strncpy(fields[i],ptr_src,ptr-ptr_src);
            ptr_src = ptr;
            i++;  
        }

        // Check for default gateway flag
        if (strcmp(fields[2], "00000000") == 0 || strcmp(fields[3], "UG") == 0 || strcmp(fields[3], "UGH") == 0) {
            if (!strcmp(fields[1],ifc)) {
                exist = true;
                break;
            }
        }
    }

    fclose(file);
    
    return exist;
}
