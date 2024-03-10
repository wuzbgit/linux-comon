#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/time.h>
#include "led.h"

#define LED_GREEN_PATH  "/sys/class/leds/green-led"
#define LED_RED_PATH     "/sys/class/leds/red-led"
#define LED_YELLOW_PATH  "/sys/class/leds/yellow-led"

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

char *led_path(e_LED led)
{
    char *path = NULL;
    switch(led) {
        case LED_GREEN:
            path = (char *)LED_GREEN_PATH;
            break;
        case LED_RED:
            path = (char *)LED_RED_PATH;
            break;
        case LED_YELLOW:
            path = (char *)LED_YELLOW_PATH;
            break;
        default:
            break;
    }

    return path;
}

int led_on(e_LED led)
{
    char *path = NULL;
    int ret;
    char buf[128] = {0};
    path = led_path(led);
    if (path == NULL) {
        return -1;
    }
    sprintf(buf,"echo heartbeat > %s/brightness",path);
    ret = led_write(path,1);

    return ret;
}

int led_off(e_LED led)
{
    char *path = NULL;
    int ret;
    char buf[128] = {0};

    path = led_path(led);
    if (path == NULL) {
        return -1;
    }
    sprintf(buf,"echo heartbeat > %s/brightness",path);
    ret = led_write(buf,0);

    return ret;
}

int led_blink(e_LED led)
{
    int ret = 0;
    char buf[128] = {0};
    char *path = NULL;
    path = led_path(led);
    if (path == NULL) {
        return -1;
    }

    sprintf(buf,"echo heartbeat > %s/trigger",path);

    if (ret == 0) {
        return system(buf);
    }
    return ret;
}
