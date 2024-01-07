#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "led.h"

#define LED_4G_PATH         "/sys/class/leds/4g/brightness"
#define IM3332_LED_PATH     "/sys/class/leds/im3332/brightness"
#define SYSTEM_LED_PATH     "/sys/class/leds/system/brightness"

int led_write(char *path, int i)
{
    char buf[24] = {0};
    size_t len;

    int fd = open(path, O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    sprintf(buf, "%d", i);
    len = strlen(buf);
    if (write(fd, buf, len) <= 0) {
        perror("write");
        close(fd);

        return -1;
    }

    close(fd);

    return 0;
}

char *led_path(e_ELEC_LED led)
{
    char *path = NULL;
    switch(led) {
        case LED_4G:
            path = (char *)LED_4G_PATH;
            break;
        case LED_IM3332:
            path = (char *)IM3332_LED_PATH;
            break;
        case LED_SYSTEM:
            path = (char *)SYSTEM_LED_PATH;
            break;
        default:
            break;
    }

    return path;
}

int led_on(e_ELEC_LED led)
{
    char *path = NULL;
    int ret;

    path = led_path(led);
    if (path == NULL) {
        return -1;
    }

    ret = led_write(path,1);

    return ret;
}

int led_off(e_ELEC_LED led)
{
    char *path = NULL;
    int ret;

    path = led_path(led);
    if (path == NULL) {
        return -1;
    }

    ret = led_write(path,0);

    return ret;
}

int led_blink(e_ELEC_LED led)
{
    int ret = 0;

    switch(led) {
        case LED_4G:
            system("echo heartbeat > /sys/class/leds/4g/trigger");
            break;
        case LED_IM3332:
            system("echo heartbeat > /sys/class/leds/im3332/trigger");
            break;
        case LED_SYSTEM:
            system("echo heartbeat > /sys/class/leds/system/trigger");
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}
