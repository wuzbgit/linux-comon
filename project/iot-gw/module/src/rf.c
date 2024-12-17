#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rf/rf_common.h"
#include "network/netip.h"
#include "network/ping.h"

#define RF_DEV_PATH     "/dev/ttyUSB1"
#define RF_MANUFACTURE "Quectel"
#define RF_MODLE        "EC200M"
#define SIM_STATE       "Inserted"
#define NET_INTF_4G     "usb0"
#define PING_DST_IP     "114.114.114.114"
#define PING_TIMEOUT_MS     1000
#define RF_RESET_GPIO       205
#define RF_POWERON_LEVEL      1

bool rf_file_exist()
{
    int ret = 0;

    ret = access(RF_DEV_PATH, F_OK);
    if (ret != 0) {
        return false;
    }

    return true;    
}

bool chardev_exist()
{
    struct stat filestat;
    int ret = 0;

    ret = access(RF_DEV_PATH, R_OK|W_OK);
    if (ret != 0) {
        return false;
    }

    if (stat(RF_DEV_PATH, &filestat) == -1) {
        return false;
    }

    if (S_ISCHR(filestat.st_mode)) {
        return true;
    }

    return false;
}

bool rf_exist()
{
    return chardev_exist();
}

void rf_dev_clean()
{
    if (chardev_exist() == true) {
        return ;
    }

    if (rf_file_exist() == true) {
        if (unlink(RF_DEV_PATH) == -1) {
            perror("Failed to delete file");
        }
    }
}

int rf_reset()
{
    int ret;
    unsigned i=0,max_cnt = 10;

    if (rf_exist()) {
        ret = poweroff_rf(RF_RESET_GPIO,!RF_POWERON_LEVEL);
        if (ret != 0) {
            perror("rf poweroff fail");

            return -1;
        }
    }
    
    i = 0;
    do {
        ret = rf_exist();
        if (ret == 0) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);

    if (i == max_cnt) {
        return -1;
    }

    ret = poweron_rf(RF_RESET_GPIO,RF_POWERON_LEVEL);
    if (ret != 0) {
        perror("rf poweron fail");

        return -1;
    }
    max_cnt = 30;
    i = 0;
    do {
        ret = rf_exist();
        if (ret == 1) {
            break;
        }
        sleep(1);
        i++;
    } while(i < max_cnt);

    if (i == max_cnt) {
        return -1;
    }

    return 0;
}

int rf_module_info(RFINFO_T *RF_info)
{
    int ret;
    int rf_mnf_ = RF_QUECTEL;

    rf_mnf_ = find_RF_manufacture(RF_DEV_PATH);
    if (rf_mnf_ == RF_NONE) {
        return -1;
    }

    init_RF_info(RF_info);

    ret = get_RF_moduleInfo(RF_DEV_PATH, RF_info, rf_mnf_);
    if (ret != 0) {
        return -1;
    }

    return 0;  
}

void rf_info_print(RFINFO_T *RF_info)
{
   printf("manufacture:%s\n",RF_info->Manufacture);
   printf("PLMN:%s\n",RF_info->PLMN);
   printf("Model:%s\n",RF_info->Model);
   printf("SN:%s\n",RF_info->SN);
   printf("IMEI:%s\n",RF_info->IMEI);
   printf("CICCID:%s\n",RF_info->CICCID);
   printf("RSSI:%d\n",RF_info->RFsignal.RSSI);
   printf("SIMStatus:%s\n",RF_info->SIMStatus);
}

int rf_module_dial()
{
    int ret;
    unsigned i=0,max_cnt = 10;
    char cmd[64] = {0};

    if (netif_exist(NET_INTF_4G) == true) {
        if (net_check_default_route(NET_INTF_4G) == false) {
            sprintf(cmd,"route add default dev %s",NET_INTF_4G);
            system(cmd);
        }        
        return 0;
    }

    poweron_rf(RF_RESET_GPIO,RF_POWERON_LEVEL);

    do {
        ret = rf_exist();
        if (ret == 1) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);
    if (i == max_cnt) {
        return -1;
    }

    ret = system("/opt/4G_call.sh");
    if (ret != 0) {
        printf("rf dial fail:can not excute /opt/4G_call.sh\n");

        return -1;
    }

    return 0;
}

int rf_module_sim_detect_enable(int enable)
{
    int ret = 0;
    int rf_mnf_ = 0;

    rf_mnf_ = find_RF_manufacture(RF_DEV_PATH);
    if (rf_mnf_ == RF_NONE) {
        return -1;
    }

    ret = rf_sim_detect(RF_DEV_PATH, rf_mnf_, enable, 1);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int rf_reset_test()
{
    int ret;
    unsigned i=0,max_cnt = 10;

    if (rf_exist()) {
        ret = poweroff_rf(RF_RESET_GPIO,!RF_POWERON_LEVEL);
        if (ret != 0) {
            perror("rf poweroff fail");

            return ret;
        }
    }
    
    i = 0;
    do {
        ret = rf_exist();
        if (ret == 0) {
            break;
        }
        sleep(1);
        i++;
    } while(i<max_cnt);

    if (i == max_cnt) {
        return -1;
    }

    ret = poweron_rf(RF_RESET_GPIO,RF_POWERON_LEVEL);
    if (ret != 0) {
        perror("rf poweron fail");

        return -1;
    }
    max_cnt = 30;
    i = 0;
    do {
        ret = rf_exist();
        if (ret == 1) {
            break;
        }
        sleep(1);
        i++;
    } while(i < max_cnt);

    if (i == max_cnt) {
        return -1;
    }

    return 0;
}

int rf_module_test()
{
    RFINFO_T RF_info;
    int ret;

    ret = rf_module_info(&RF_info);
    if (ret != 0) {
        return -1;
    }

    if (strcmp(RF_info.Manufacture,RF_MANUFACTURE) || strcmp(RF_info.Model,RF_MODLE)) {
        return -1;
    }

    return 0;  
}

int rf_simdetect_test()
{
    int ret;
    RFINFO_T RF_info;
        
    ret = rf_module_sim_detect_enable(1);
    if (ret != 0) {
        printf("rf enable sim card detect error\n");
        return -1;
    }

    while(1) {
        ret = rf_module_info(&RF_info);
        if (ret == 0) {
            printf("sim card state:%s\n",RF_info.SIMStatus);
        }

        usleep(3000 * 1000);
    }         
}

int rf_net_test()
{
    int ret;
    char cmd[64] = {0};
    if (netif_exist(NET_INTF_4G) == true) {
        if (net_check_default_route(NET_INTF_4G) == false) {
            sprintf(cmd,"route add default dev %s",NET_INTF_4G);
            system(cmd);
        }
    }
    else {
        printf("rf net test fail:4G net interface %s does not exist\n",NET_INTF_4G);
        return -1;
    }
    
    ret = ping_intf(PING_DST_IP, NET_INTF_4G, 0, 6, PING_TIMEOUT_MS);
    if (ret <= 0) {
        printf("rf net test fail:ping %s failed\n",PING_DST_IP);
        return -1;
    }
    
    return 0;
}