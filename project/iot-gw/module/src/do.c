#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>  
#include <getopt.h>
#include "gpio/gpio.h"

#define PORT_DO_CHANNEL_0  100
#define PORT_DO_CHANNEL_1  101
#define PORT_DO_CHANNEL_2  102
#define PORT_DO_CHANNEL_3  103

#define DO_CHANNEL_0       "PD4"
#define DO_CHANNEL_1       "PD5"
#define DO_CHANNEL_2       "PD6"
#define DO_CHANNEL_3       "PD7"

struct do_channel
{
    int channel;
    char *port_path;
    int port;
};

static struct do_channel do_chns[] = {
    {
        .channel = 0,
        .port_path = DO_CHANNEL_0,
        .port = PORT_DO_CHANNEL_0
    },
    {
        .channel = 1,
        .port_path = DO_CHANNEL_1,
        .port = PORT_DO_CHANNEL_1
    },
    {
        .channel = 2,
        .port_path = DO_CHANNEL_2,
        .port = PORT_DO_CHANNEL_2
    },
    {
        .channel = 3,
        .port_path = DO_CHANNEL_3,
        .port = PORT_DO_CHANNEL_3
    },           
};

static struct do_channel* do_get_chn(int channel)
{
    int i;
    int array = sizeof(do_chns) / sizeof(do_chns[0]);
    
    for (i = 0; i < array; i++) {
        if (do_chns[i].channel == channel) {
            return &do_chns[i];
        }
    }

    return NULL;
}

static int gpio_set_out(struct do_channel* do_chn)
{
    char cmd[128] = {0};
    int ret;

    sprintf(cmd,"gpio -s %s 1 > /dev/null",do_chn->port_path);
    ret = system(cmd);
    if (ret != 0) {
        return -1;
    }

    ret = gpio_export(do_chn->port);
    if (ret != 0) {
        return -1;
    }

    ret = gpio_set_direction(do_chn->port, GPIO_DIR_OUT);
    if (ret != 0) {
        return -1;
    }

    return ret;
}

int do_init()
{
    int ret;
    int i;

    for (i = 0; i < sizeof(do_chns)/sizeof(do_chns[0]); i++) {
        ret = gpio_set_out(&do_chns[i]);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;  
}

int do_out(int channel, int level)
{
    struct do_channel* do_chn = NULL;
    int ret = 0;
    
    do_chn = do_get_chn(channel);
    if (do_chn == NULL) {
        return -1;
    }

    ret = gpio_set_level(do_chn->port, level);
    if (ret < 0) {
        return -1;
    }

    return 0;
}
