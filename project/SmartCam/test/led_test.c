#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "led.h"

static int led_blink_test()
{
    int ret = 0;

    ret = led_blink(LED_USER0);
    ret |= led_blink(LED_USER1);
    ret |= led_blink(LED_USER2);
    
    return ret;
}

int main(int argc,char *argv[])
{
    int ret = 0;
    ret = led_blink_test();
    if (ret != 0) {
        printf("led test fail\n");
    }
    else {
        printf("led test success\n");
    }

    return ret;
}
