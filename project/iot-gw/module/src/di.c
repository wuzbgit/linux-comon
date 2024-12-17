#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>  
#include "gpio/gpio.h"

#define DI_CHANNEL_0       "PD8"
#define DI_CHANNEL_1       "PD9"
#define DI_CHANNEL_2       "PD10"
#define DI_CHANNEL_3       "PD11"

#define PORT_DI_CHANNEL_0   104
#define PORT_DI_CHANNEL_1   105
#define PORT_DI_CHANNEL_2   106
#define PORT_DI_CHANNEL_3   107

struct di_channel {
    int channel;
    char *port_path;
    int port;
};

static struct di_channel di_chns[] = {
    {
        .channel = 0,
        .port_path = DI_CHANNEL_0,
        .port = PORT_DI_CHANNEL_0
    },
    {
        .channel = 1,
        .port_path = DI_CHANNEL_1,
        .port = PORT_DI_CHANNEL_1
    },
    {
        .channel = 2,
        .port_path = DI_CHANNEL_2,
        .port = PORT_DI_CHANNEL_2
    },
    {
        .channel = 3,
        .port_path = DI_CHANNEL_3,
        .port = PORT_DI_CHANNEL_3
    },      
};

static int gpio_set_in(struct di_channel* di_chn)
{
    char cmd[128] = {0};
    int ret;

    sprintf(cmd,"gpio -s %s 0 > /dev/null",di_chn->port_path);
    ret = system(cmd);
    if (ret != 0) {
        return -1;
    }

    ret = gpio_export(di_chn->port);
    if (ret != 0) {
        return -1;
    }

    ret = gpio_set_direction(di_chn->port, GPIO_DIR_IN);
    if (ret != 0) {
        return -1;
    }

    return ret;
}

static struct di_channel* di_get_chn(int channel)
{
    int i;
    int array = sizeof(di_chns) / sizeof(di_chns[0]);
    
    for (i = 0; i < array; i++) {
        if (di_chns[i].channel == channel) {
            return &di_chns[i];
        }
    }

    return NULL;
}

int di_init()
{
    int ret;
    int i;

    for (i = 0; i < sizeof(di_chns)/sizeof(di_chns[0]); i++) {
        ret = gpio_set_in(&di_chns[i]);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;  
}

int di_read(int channel, int *level)
{
    struct di_channel* di_chn = NULL;
    int ret = 0;

    di_chn = di_get_chn(channel);
    if (di_chn == NULL) {
        return -1;
    }

    ret = gpio_get_level(di_chn->port, level);
    if (ret < 0) {
        return -1;
    }
    
    return 0;
}
