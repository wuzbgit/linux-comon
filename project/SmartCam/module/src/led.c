#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "led.h"

struct iotgw_leds {
    e_IOTGW_LED led;
    char *led_path;
};

static struct iotgw_leds leds[] = {
    {
        .led = LED_USER0,
        .led_path = "/sys/class/leds/user-led0"
    },
    {
        .led = LED_USER1,
        .led_path = "/sys/class/leds/user-led1"
    },
    {
        .led = LED_USER2,
        .led_path = "/sys/class/leds/user-led2"
    },
    {
        .led = LED_USER3,
        .led_path = "/sys/class/leds/user-led3"
    },
    {
        .led = LED_USER4,
        .led_path = "/sys/class/leds/user-led4"
    }                
};

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

int led_brightness_path(e_IOTGW_LED led,char *led_path)
{
    int i;
    unsigned cnt = sizeof(leds)/sizeof(leds[0]);
    char *path = NULL;

    for (i=0;i<cnt;i++) {
        if (leds[i].led == led) {
            sprintf(led_path,"%s/brightness",leds[i].led_path);
            path = led_path;
            break;
        }
    }

    return led_path;
}

int led_on(e_IOTGW_LED led)
{
    char *path = NULL;
    char led_path[128] = {0};
    int ret;

    path = led_brightness_path(led,led_path);
    if (path == NULL) {
        return -1;
    }

    ret = led_write(path,1);

    return ret;
}

int led_off(e_IOTGW_LED led)
{
    char *path = NULL;
    char led_path[128] = {0};
    int ret;

    path = led_brightness_path(led,led_path);
    if (path == NULL) {
        return -1;
    }

    ret = led_write(path,0);

    return ret;
}

int led_blink(e_IOTGW_LED led)
{
    int ret = 0;
    int i;
    char cmd[128] = {0};
    unsigned cnt = sizeof(leds)/sizeof(leds[0]);

    for (i=0;i<cnt;i++) {
        if (leds[i].led == led) {
            sprintf(cmd,"echo heartbeat > %s/trigger",leds[i].led_path);
            break;
        }
    }

    if (i != cnt) {
        system(cmd);
    }
    else {
        ret = -1;
    }

    return ret;
}

int led_blink_test()
{
    int ret = 0;

    ret = led_blink(LED_USER0);
    ret |= led_blink(LED_USER1);
    ret |= led_blink(LED_USER2);
    ret |= led_blink(LED_USER3);
    ret |= led_blink(LED_USER4);
    
    return ret;
}