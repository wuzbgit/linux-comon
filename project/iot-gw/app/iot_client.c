#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>
#include "iot_daemon.h"

#define PORT 9091
#define BUFFER_SIZE 1024

int client_request(void *sndbuf,unsigned sndlen,void *recvbuf,unsigned recvlen)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    send(sock, sndbuf, sndlen, 0);

    int valread = read(sock, recvbuf, recvlen);

    close(sock);

    return valread;
}

static int rtc_get()
{
    struct test_package *sndpack = NULL;
    struct test_package *recvpack = NULL;
    unsigned len = 0;
    int ret;

    len = sizeof(struct test_package) + sizeof(struct rtc_info);

    sndpack = malloc(len);
    if (sndpack == NULL) {
        return -1;
    }

    recvpack = malloc(len);
    if (recvpack == NULL) {
        return -1;
    }

    sndpack->cmd = CMD_RTC_INIT;
    ret = client_request(sndpack,len,recvpack,len);
    if ((ret != len) || (recvpack->state != 0)) {
        printf("rtc get request fail\n");
        free(sndpack);
        free(recvpack);
        return -1;
    }

    rtc_info_t *info = recvpack->payload;
    printf("rtc time:%04d-%02d-%02d %02d:%02d:%02d\n",info->year,info->month,info->day,info->hour,info->min,info->sec);

    free(sndpack);
    free(recvpack);

    return 0;
}

static int ai_get_channel(int channel,int *value)
{
    struct test_package *sndpack = NULL;
    struct test_package *recvpack = NULL;
    unsigned len = 0;
    int ret;
    ai_info_t *info;
    int i;

    len = sizeof(struct test_package) + sizeof(struct di_info);

    sndpack = malloc(len);
    if (sndpack == NULL) {
        return -1;
    }

    recvpack = malloc(len);
    if (recvpack == NULL) {
        return -1;
    }

    sndpack->cmd = CMD_RTC_INIT;
    info = sndpack->payload;
    info->ai_num = channel;
    
    ret = client_request(sndpack,len,recvpack,len);
    if ((ret != len) || (recvpack->state != 0)) {
        printf("di get request fail\n");
        free(sndpack);
        free(recvpack);
        return -1;
    }

    info = recvpack->payload;
    *value = info->data;

    free(sndpack);
    free(recvpack);

    return 0;    
}

static int ai_get()
{
    int i;
    int value = 0;
    int ret = 0;
    printf("ai read value:\n");
    for (i = 0;i < 4;i++) {
        ret = ai_get_channel(i,&value);
        if (ret = 0) {
            print("%d:%d\n",i,value);
        }
    }
}

static int di_get()
{
    struct test_package *sndpack = NULL;
    struct test_package *recvpack = NULL;
    unsigned len = 0;
    int ret;
    di_info_t *info;
    int i;

    len = sizeof(struct test_package) + sizeof(struct di_info);

    sndpack = malloc(len);
    if (sndpack == NULL) {
        return -1;
    }

    recvpack = malloc(len);
    if (recvpack == NULL) {
        return -1;
    }

    sndpack->cmd = CMD_RTC_INIT;
    info = sndpack->payload;
    info->di_mask = 0xF;
    
    ret = client_request(sndpack,len,recvpack,len);
    if ((ret != len) || (recvpack->state != 0)) {
        printf("di get request fail\n");
        free(sndpack);
        free(recvpack);
        return -1;
    }

    printf("di value:");
    for (i = 0; i < 4; i++) {
        printf("%d ",((info->level >> (4 - i)) & 0x1);
    }
    printf("\n");

    free(sndpack);
    free(recvpack);

    return 0;    
}

static int do_set(unsigned level)
{
    struct test_package *sndpack = NULL;
    struct test_package *recvpack = NULL;
    unsigned len = 0;
    int ret;
    do_info_t *info;
    int i;
    
    len = sizeof(struct test_package) + sizeof(struct di_info);

    sndpack = malloc(len);
    if (sndpack == NULL) {
        return -1;
    }

    recvpack = malloc(len);
    if (recvpack == NULL) {
        return -1;
    }

    sndpack->cmd = CMD_RTC_INIT;
    info = sndpack->payload;
    info->do_mask = 0xF;
    info->level = level;

    ret = client_request(sndpack,len,recvpack,len);
    if ((ret != len) || (recvpack->state != 0)) {
        printf("di get request fail\n");
        free(sndpack);
        free(recvpack);
        return -1;
    }

    printf("do set value=%d success\n");

    free(sndpack);
    free(recvpack);

    return 0;    
}
static void display_usage()
{
    printf(
        "iot_client  : iot client test\n"
        "Usage      : ./iot_client [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --module,            -m         - rtc,ai,di,do\n"
        "    --level,             -l         - set do value\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"module"        , required_argument, NULL, 'm'},
    {"level"        , required_argument, NULL, 'l'},
    {"help"          , required_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int level = 0;
    int cmd = -1;
    char module[64] = {0};

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"m:l:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'm':
            strncpy(module,optarg,sizeof(module) - 1);                      
            break;
        case 'l':
            level = atoi(optarg);
            cmd = 0;
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }

    if (!strcmp(optarg,"rtc")) {
        rtc_get();
    }
    else if (!strcmp(optarg,"di")) {
        di_get();
    }
    else if (!strcmp(optarg,"ai")) {
        ai_get();
    }
    else if (!strcmp(optarg,"do")) {
        do_set(level);
    }  

    return 0;
}