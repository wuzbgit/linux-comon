#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

extern int rtc_unit_test();

#define ETH0        "eth0"
#define ETH0_DST_IP "192.168.11.150"

#define ETH1        "eth1"
#define ETH1_DST_IP "192.168.12.150"

#define PING_TIMEOUT_MS 500

extern int rs485_echo_test(char *devpath);
extern int ai_read_channel(int channel, int *value);
extern int read_current(int channel, float *cur);
extern int di_init();
extern int di_read(int channel, char *level);
extern int do_init();
extern int do_out(int channel, int level);

static int rtc_test()
{
    int ret;
    printf("rtc testing...\n");
    ret = rtc_unit_test();
    if (ret == 0) {
        printf("rtc test success\n");
    }
    else {
        printf("rtc test fail\n");
    }

    return ret;
}

static int dio_test()
{
    int i,level,ret;
    int lvl = 1;
    int io_cnt = 0;
    bool dio_state = true;

    printf("dio testing...\n");

    di_init();
    do_init();

    while (1)
    {
        for ( i = 0; i < 4; i++) {
            ret = do_out(i, lvl);
            if (ret != 0) {
                printf("DO Port[%d] set level=%d fail\n",i,lvl);

                return ret;
            }           
        }

        usleep(200 * 1000);

        for ( i = 0; i < 4; i++) {
            ret = di_read(i, &level);
            if (ret != 0) {
                printf("DI Port[%d] read fail\n",i);
                dio_state = false;
            }
            else {
                if (level != lvl) {
                    printf("di port[%d] fail:expect level=%d,actual level=%d\n",i,lvl,level);
                    dio_state = false;
                }
            }
        }

        lvl = !lvl;

        if ((dio_state == false) || (io_cnt >= 4)) {
            break;
        }
        io_cnt ++;
        usleep(200 * 1000);                             
    }

    if (dio_state == true) {
        printf("dio test success\n");
    }
    else {
        printf("dio test fail\n");
    }

    return (dio_state == false) ? -1 : 0;
}

static int eth_ping_test(char *intf,char *dest_ip)
{
    int ret;
    printf("%s ping testing...\n",intf);
    if (netif_exist(intf) == false) {
        printf("%s does not exist\n",intf);
        return -1;
    }

    ret = ping_intf(dest_ip, intf, 0, 6, PING_TIMEOUT_MS);
    if (ret <= 0) {
        printf("%s ping %s fail\n",intf,dest_ip);
        return -1;
    }
    
    printf("%s ping %s success\n",intf,dest_ip);
    return 0;    
}

static bool rf_end = false;
#include "rf/rf_common.h"
void* thn_rf_test(void* arg)
{
    int ret;
    
    printf("rf testing...\n");
    ret = rf_reset_test();
    if (ret != 0) {
        printf("rf reset test fail\n");
    }
    else {
        printf("rf reset test success\n"); 
    }

    ret = rf_module_test();
    if (ret != 0) {
        printf("rf module test fail:invalid rf module\n");
    }
    else {
        printf("rf module test succss\n");
    }

    // RFINFO_T RF_info;
    // ret = rf_module_info(&RF_info);
    // if (ret == 0) {
    //     rf_info_print(&RF_info);
    // }  
    rf_end = true;
    ret = rf_simdetect_test();
    if (ret != 0) {
       printf("rf sim card detect test fail\n"); 
    }
    else {
        printf("rf sim card detect test success\n");
    }

    return ret;
}

void* thn_ai_test(void* arg)
{
    int i,ret;
    float cur;
    int value = 0;

    while(1) {
        for ( i = 0; i < 2; i++) {
            ret = ai_read_channel(i, &value);
            if (ret == 0) {
                printf("AI Channel %d value:%d,current=%f mA\n", i, value, 12.26*value/1000);
            }
        }

        usleep(2000*1000);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    pthread_t thread1, thread2;

    ret = led_blink_test();
    if (ret != 0) {
        printf("led test fail\n");
    }
    else {
        printf("led test success\n");
    }

    rtc_test();

    dio_test();

    eth_ping_test(ETH0, ETH0_DST_IP);

    eth_ping_test(ETH1, ETH1_DST_IP);

    if (rs485_loopback_test("/dev/ttyAS1","/dev/ttyAS3") == 0) {
        printf ("rs485 %s send %s receive test success\n","/dev/ttyAS1","/dev/ttyAS3");
    }
    else {
        printf ("rs485 %s send %s receive test fail\n","/dev/ttyAS1","/dev/ttyAS3");
    }

    if (rs485_loopback_test("/dev/ttyAS3","/dev/ttyAS1") == 0) {
        printf ("rs485 %s send %s receive test success\n","/dev/ttyAS3","/dev/ttyAS1");
    }
    else {
        printf ("rs485 %s send %s receive test fail\n","/dev/ttyAS3","/dev/ttyAS1");
    }

    ret = pthread_create(&thread1, NULL, thn_rf_test, NULL);
    if (ret != 0) {
        return -1;
    }
    
    while (rf_end == false) {
        usleep(500*1000);
    }
    
    ret = pthread_create(&thread2, NULL, thn_ai_test, NULL);
    if (ret != 0) {
        return -1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
