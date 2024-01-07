#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utest/utest.h"
#include "rf/rf_common.h"
#include "network/netip.h"
#include "network/ping.h"

#define RF_DEV_PATH     "/dev/ttyUSB2"
static int rf_mnf_ = RF_QUECTEL;
static int test_mode = 0;
#define RF_MANUFACTURE "Quectel"
#define RF_MODLE        "EC20F"
#define SIM_STATE       "Inserted"
#define SIM_TYPE        "dianxin"

static int rf_module_tc_init(struct utest_tc_export *tc)
{
    int ret = RF_NONE;
    int stat = -1;

    ret = find_RF_manufacture(RF_DEV_PATH);
    if (ret == RF_LYNQ) {
        stat = 0;
    }
    else if  (ret == RF_QUECTEL) {
        stat = 0;
    }
    else {
        printf("can not detect rf device\n");
    }

    rf_mnf_ = ret;

    return stat;
}

static void rf_module_tc_test(struct utest_tc_export *tc)
{
    RFINFO_T RF_info;
    int res;

    init_RF_info(&RF_info);

    get_RF_moduleInfo(RF_DEV_PATH, &RF_info, rf_mnf_);

    if (strcmp(RF_info.Manufacture,RF_MANUFACTURE) || strcmp(RF_info.Model,RF_MODLE)) {
        printf("invalid rf module\n");
        uassert_true(false);
        return;
    }

    if (!test_mode) {
        print_RF_status(&RF_info);
    }

    if (strcmp(RF_info.SIMStatus,SIM_STATE)) {
        printf("%s:can not detect sim card\n",__func__);
        uassert_true(false);
        return;
    }

    uassert_true(true);   
}

static int rf_module_tc_exit(struct utest_tc_export *tc)
{
    rf_mnf_ = RF_NONE;

    return 0;
}

static int rf_reset_tc_init(struct utest_tc_export *tc)
{
    char dev_[64] = {0};
    int ret;
    unsigned i=0,max_cnt = 10;

    poweron_rf();
    do {
        ret = RF_device_detect(dev_, 63);
        if ((ret == 0) && (!strcmp(dev_,"/dev/ttyUSB3"))) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);
    if (i == max_cnt) {
        return -1;
    }

    return 0;
}

static void rf_reset_tc_test(struct utest_tc_export *tc)
{
    char dev_[64] = {0};
    int ret;
    unsigned i=0,max_cnt = 10;

    poweroff_rf();
    i = 0;
    do {
        ret = RF_device_detect(dev_, 63);
        if (ret != 0) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);
    if (i == max_cnt) {
        uassert_true(false);
        return ;
    }

    poweron_rf();
    memset(dev_,0,sizeof(dev_));
    i = 0;
    do {
        ret = RF_device_detect(dev_, 63);
        if ((ret == 0) && (!strcmp(dev_,"/dev/ttyUSB3"))) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);
    if (i == max_cnt) {
        uassert_true(false);
        return ;
    }

    int rf_mnf_;
    rf_mnf_ = find_RF_manufacture(dev_);
    if (rf_mnf_ == RF_NONE) {
        uassert_true(false);
    }

    ret = rf_at_test(dev_, rf_mnf_);
    if (ret != 0) {
        uassert_true(false);
    }
    else {
        uassert_true(true);
    }
}

static int rf_reset_tc_exit(struct utest_tc_export *tc)
{
    rf_mnf_ = RF_NONE;

    return 0;
}

#define NET_INTF_4G     "ppp0"
#define PING_DST_IP     "8.8.8.8"
#define PING_TIMEOUT_MS 1000
#define NET_ETH_INTF    "eth1"

static int rf_net_tc_init(struct utest_tc_export *tc)
{
    char dev_[64] = {0};
    int ret;
    unsigned i=0,max_cnt = 10;

    while (1) {
        if (net_check_default_route(NET_ETH_INTF) == false) {
            break;            
        }
        system("route del default"); 
    }

    if (netif_exist(NET_INTF_4G) == true) {
        if (net_check_default_route(NET_INTF_4G) == false) {
            system("route add default dev ppp0");
        }        
        return 0;
    }

    poweron_rf();
    do {
        ret = RF_device_detect(dev_, 63);
        if ((ret == 0) && (!strcmp(dev_,"/dev/ttyUSB3"))) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);
    if (i == max_cnt) {
        return -1;
    }

    RFINFO_T RF_info;
    int res;

    init_RF_info(&RF_info);

    get_RF_moduleInfo(RF_DEV_PATH, &RF_info, RF_QUECTEL);

    if (strcmp(RF_info.SIMStatus,SIM_STATE)) {
        printf("%s:can not detect sim card\n",__func__);
        return -1;
    }

    if (access("/var/lock/LCK..ttyUSB2",F_OK) == 0) {
        system("rm /var/lock/LCK..ttyUSB2");
    }
    if (access("/var/lock/LCK..ttyUSB3",F_OK) == 0) {
        system("rm /var/lock/LCK..ttyUSB3");
    }    
    return 0;
}

static void rf_net_tc_test(struct utest_tc_export *tc)
{
    int ret;
    char cmd[64] = {0};

    if (netif_exist(NET_INTF_4G) == false) {
        if (!strcmp("yidong",SIM_TYPE)) {
            sprintf(cmd,"./4G_yidong.sh > /dev/null");
        }
        else if (!strcmp("dianxin",SIM_TYPE)) {
            sprintf(cmd,"./4G_dianxi.sh > /dev/null");
        }
        else if (!strcmp("liantong",SIM_TYPE)) {
            sprintf(cmd,"./4G_liantong.sh > /dev/null");
        }
        else {
            sprintf(cmd,"./4G_call.sh.sh > /dev/null");
        }        
        ret = system("./4G_call.sh > /dev/null");
        if (ret != 0) {
            printf("4G dialog failed\n");
            uassert_true(false);
            return;
        }
    }

    while (1) {
        if (net_check_default_route(NET_ETH_INTF) == false) {
            break;            
        }
        system("route del default"); 
    }

    if (netif_exist(NET_INTF_4G) == true) {
        if (net_check_default_route(NET_INTF_4G) == false) {
            system("route add default dev ppp0");
        }
    }
    else {
        printf("4G net interface does not exist\n");
        uassert_true(false);
        return;
    }
    
    ret = ping(PING_DST_IP, 0, 6,PING_TIMEOUT_MS);
    if (ret <= 0) {
        printf("ping %s failed\n",PING_DST_IP);
        uassert_true(false);
        return ;
    }
    uassert_true(true);
}

static int rf_net_tc_exit(struct utest_tc_export *tc)
{
    return 0;
}
static struct utest_tc_export g_rf_test[] = {
    {
        .module = "rf",
        .name = "reset_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = rf_reset_tc_init,
        .tc         = rf_reset_tc_test,
        .cleanup    = rf_reset_tc_exit
    },     
    {
        .module = "rf",
        .name = "module_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = rf_module_tc_init,
        .tc         = rf_module_tc_test,
        .cleanup    = rf_module_tc_exit
    },   
    {
        .module = "rf",
        .name = "net_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = rf_net_tc_init,
        .tc         = rf_net_tc_test,
        .cleanup    = rf_net_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_rf_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_rf_test[i].name);
    }
}

int func_rf_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_rf_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        test_mode = 1;
        for (i=0;i<tc_count;i++) {
            utest_run(&g_rf_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_rf_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }
        else {
            utest_run(&g_rf_test[i],loop_count);
        } 
    }

    return 0;
}


