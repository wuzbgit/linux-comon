#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/rtc.h>
#include "threads/thread.h"
#include "rf/rf_common.h"
#include "iot_daemon.h"

#define BUFFER_SIZE 1024

static int rtc_get_info(rtc_info_t *info)
{
    int ret = 0;
    struct rtc_time tm;
    ret = read_rtc_time(&tm);
    if (ret == 0) {
        info->year = tm.tm_year;
        info->month = tm.tm_mon;
        info->day = tm.tm_mday;
        info->hour = tm.tm_hour;
        info->min = tm.tm_min;
        info->sec = tm.tm_sec;
    }
    
    return ret;
}

static int ret_set_info(rtc_info_t *info)
{
    int ret;
    struct rtc_time tm;
    tm.tm_year = info->year;
    tm.tm_mon = info->month;
    tm.tm_mday = info->day;
    tm.tm_hour = info->hour;
    tm.tm_min = info->min;
    tm.tm_sec = info->sec;
    ret = rtc_set(tm);
    
    return ret;
}

static int led_operation(led_info_t *info)
{
    int ret = -1;
    switch (info->opt)
    {
    case OPT_LED_ON:
        ret = led_on(info->led_num);
        break;
    case OPT_LED_OFF:
        ret = led_off(info->led_num);
        break;
    case OPT_LED_BLINK:
        ret = led_blink(info->led_num);
        break;
    default:
        break;
    }

    return ret;
}

static int do_set_info(do_info_t *info)
{
    int ret;
    char level;
    int i;

    for (i = 0;i < 16; i++) {
        if (info->do_mask & (1 << i)) {
            ret = do_out(i, info->level);
        }
    }

    return ret;
}

static int di_get_info(di_info_t *info)
{
    int ret;
    char level;
    int i;
    info->level = 0;
    for (i = 0;i < 16; i++) {
        if (info->di_mask & (1 << i)) {
            ret = di_read(i, &level);
            if (ret == 0) {
                info->level |= (level << i);
            }
        }
    }

    return ret;
}

static int ai_get_info(ai_info_t *info)
{
    int ret;
    float cur = 0;
    ret = read_current(info->ai_num, &cur);
    if (ret == 0) {
        info->data = cur * 100;
    }

    return ret;
}

static int rf_mod_info(rf_mod_info_t *info)
{
    int ret = 0;
    RFINFO_T RF_info;
    ret = rf_module_info(&RF_info);
    if (ret == 0) {
        memset(info, 0, sizeof(rf_mod_info));
        strcpy(info->Manufacture, RF_info.Manufacture);
        strcpy(info->PLMN, RF_info.PLMN);
        strcpy(info->Model, RF_info.Model);
        strcpy(info->SN, RF_info.SN);
        strcpy(info->IMEI, RF_info.IMEI);
        strcpy(info->CICCID, RF_info.CICCID);
        strcpy(info->SIMStatus, RF_info.SIMStatus);
    }

    return ret;
}

#define MAX_RS485_NUM       2
struct rs485_thread_instance
{
    pthread_t rs485_thread;
    char devpath[32];
};

static struct rs485_thread_instance rs485_instance[MAX_RS485_NUM] = {
    {
        .rs485_thread = NULL,
        .devpath = "/dev/ttyAS1"
    },
    {
        .rs485_thread = NULL,
        .devpath = "/dev/ttyAS3"
    }
};

static void* rs485_test_thread(void* arg) {
    char *devpath = (char *)arg;
    int ret = 0;

    ret = rs485_echo_test(devpath);
    if (ret != 0) {

    }
    printf("Hello from the thread!\n");
    return NULL;
}
static int rs485_test(struct rs485_test_info *info)
{
    int ret = -1;
    struct rs485_thread_instance *instance = NULL;
    if (info->instance >= MAX_RS485_NUM) {
        printf("invalid instance\n");

        return -1;
    }
    
    switch (info->instance)
    {
    case 0:
        instance = &rs485_instance[0];
        ret = 0;
        break;
    case 1:
        instance = &rs485_instance[1];
        ret = 0;
        break;
    default:
        break;
    }

    if (info->enable && instance != NULL) {
        ret = pthread_create(&rs485_instance->rs485_thread, NULL, rs485_test_thread, rs485_instance->devpath);
    }

    if (info->enable == 0 && instance != NULL) {
        if (instance->rs485_thread) {
            pthread_cancel(instance->rs485_thread);
            pthread_join(instance->rs485_thread, NULL);
        }
    }

    return ret;
}

int test_process(int socket_fd, struct test_package *pack, unsigned len)
{
    int ret = 0;
    int state = -1;

    switch (pack->cmd)
    {
    case CMD_RTC_INIT:
        ret = rtc_init();
        if (ret == 0) {
            pack->state = 0;
            pack->payloadlen = 0;
        }
        break;
    case CMD_RTC_GET:
        ret = rtc_get_info(pack->payload);
        if (ret == 0) {
            pack->state = 0;
            pack->payloadlen = sizeof(rtc_info_t);
        }
        break;
    case CMD_RTC_SET:
        if (pack->payloadlen == sizeof(rtc_info_t)) {
            ret = ret_set_info(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = 0;
            }
        }
        break;
    case CMD_LED_SET:
        if (pack->payloadlen == sizeof(led_info_t)) {
            ret = led_operation(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = 0;
            }
        }
        break;
    case CMD_DO_SET:
        if (pack->payloadlen == sizeof(do_info_t)) {
            ret = do_set_info(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = 0;
            }
        }
        break;
    case CMD_DI_GET:
        if (pack->payloadlen == sizeof(di_info_t)) {
            ret = di_get_info(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = sizeof(di_info_t);
            }
        }
        break;
    case CMD_AI_GET:
        if (pack->payloadlen == sizeof(ai_info_t)) {
            ret = ai_get_info(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = sizeof(ai_info_t);
            }
        }
        break;
    case CMD_RS485_TEST:
        if (pack->payloadlen == sizeof(rs485_test_info_t)) {
            ret = rs485_test(pack->payload);
            if (ret == 0) {
                pack->state = 0;
                pack->payloadlen = sizeof(rs485_test_info_t);
            }
        }
        break;
    case CMD_RS422_TEST:
        break;
    case CMD_RF_RESET:
        ret = rf_reset();
        if (ret == 0) {
            pack->state = 0;
            pack->payloadlen = 0;
        }
        break;
    case CMD_RF_MOD_GET:
        ret = rf_mod_info(pack->payload);
        if (ret == 0) {
            pack->state = 0;
            pack->payloadlen = sizeof(rf_mod_info_t);
        }
        break;
    case CMD_RF_SIM_GET:
        break;
    case CMD_RF_SIM_DETECT_SET:
        if (pack->payloadlen == sizeof(unsigned int)) {
            ret = rf_module_sim_detect_enable(*((unsigned *)pack->payload));
            if (ret == 0) {
                pack->state = 0;
            }
        }
        break;
    default:
        break;
    }

    if (pack->state != 0) {
        pack->payloadlen = 0;
    }

    send(socket_fd, pack, sizeof(pack) + pack->payloadlen, 0);

    return pack->state;
}

int test_loop(int client_fd)
{
    int len = 0;
    char buffer[BUFFER_SIZE] = {0};

    while (1) {
        len = read(client_fd, buffer, BUFFER_SIZE);

        test_process(client_fd, buffer, len);
    }

}

int main_loop()
{  
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return EXIT_FAILURE;
    }  
  
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SERVER_PROT);
  
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }
  
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }
  
    printf("Server is listening on port %d\n", SERVER_PROT);  
  
    while (1) {  
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {  
            perror("accept");  
            continue;  
        }  
  
        printf("Connection accepted from %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
  
        test_loop(new_socket);

        close(new_socket);
    }

    close(server_fd);  
    return 0;  
}

int main(int argc, char *argv[])
{
    return 0;
}