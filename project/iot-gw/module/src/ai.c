#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>

#define AI_CHANNEL_0    "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define AI_CHANNEL_1    "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define AI_CHANNEL_2    "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define AI_CHANNEL_3    "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"

static int read_sys(char *syspath,char *buf,unsigned bufsize)
{
    FILE *fp;
    fp = fopen(syspath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    fgets(buf, bufsize - 1, fp);

    fclose(fp);

    return 0;    
}

int ai_read_channel(int channel, int *value)
{
    char *channel_path = NULL;
    int ret;
    char buf[32] = {0};

    switch (channel)
    {
    case 0:
        channel_path = AI_CHANNEL_0;
        break;
    case 1:
        channel_path = AI_CHANNEL_1;
        break;
    case 2:
        channel_path = AI_CHANNEL_2;
        break;
    case 3:
        channel_path = AI_CHANNEL_3;
        break;
    default:
        break;
    }

    if(channel_path == NULL) {
        return -1;
    }

    ret = read_sys(channel_path,buf,sizeof(buf) - 1);
    if (ret == 0) {
        *value = atoi(buf);
    }
    return ret;
}

int read_current(int channel, float *cur)
{
    int ret = 0;
    int value = 0;

    ret = ai_read_channel(channel, &value);
    if (ret == 0) {
        *cur = value * 12.26/1000;
    }
}
