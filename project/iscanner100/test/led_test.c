#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utest/utest.h"
#include "led.h"

static int led_tc_init(struct utest_tc_export *tc)
{

    return 0;
}

static void led_tc_test(struct utest_tc_export *tc)
{
    led_blink(LED_GREEN);
    led_blink(LED_RED);
    led_blink(LED_YELLOW);

    uassert_true(true);
}

int led_tc_exit(struct utest_tc_export *tc)
{
    return 0;
}

static struct utest_tc_export g_led_test[] = {
    {
        .module = "led",
        .name = "blink_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = led_tc_init,
        .tc         = led_tc_test,
        .cleanup    = led_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_led_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_led_test[i].name);
    }
}

int func_led_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_led_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_led_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_led_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_led_test[i],loop_count);
    }

    return 0;
}
