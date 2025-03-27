#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sys_rw.h"

#define BOC_DO_PATH     "/sys/class/misc/do/value"

int do_set_value(int value)
{
    int ret = -1;
    char buf[2] = {0};
    if (value == 0) {
        buf[0] = '0';
    }
    else if (value == 1) {
        buf[0] = '1';
    }
    else {
        return -1;
    }

    ret = write_sysfs_string(BOC_DO_PATH, "1");

    return ret;
}

int do_get_value(int *value)
{
    int ret = -1;
    int len;
    char buf[4] = {0};

    len = read_sysfs_string(BOC_DO_PATH, buf, 4);
    if (len == 2) {
        if (buf[0] == '0') {
            *value = 0;
            ret = 0;
        }
        else if (buf[0] == '1') {
            *value = 1;
            ret = 0;
        }
        else {
            return -1;
        }
    }

    return ret;
}
