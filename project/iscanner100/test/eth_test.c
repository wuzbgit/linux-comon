#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utest/utest.h"
#include "network/netip.h"
#include "network/ping.h"

#define ETH0_INTERFACE   "eth0"
#define ETH1_INTERFACE   "eth1"
#define PINT_DEST_IP    "192.168.12.150"
#define PING_TIMEOUT_MS 1000
static int eth0_tc_init(struct utest_tc_export *tc)
{
    if (netif_exist(ETH0_INTERFACE) == false) {
        printf("eth0 does not exist\n");
        return -1;
    }

    tc->priv = ETH0_INTERFACE;

    return 0;
}

static int eth1_tc_init(struct utest_tc_export *tc)
{
    if (netif_exist(ETH1_INTERFACE) == false) {
        printf("eth0 does not exist\n");
        return -1;
    }

    tc->priv = ETH1_INTERFACE;

    return 0;
}

static void eth_tc_test(struct utest_tc_export *tc)
{
    int ret;
    if (tc->priv == NULL) {
        uassert_true(false);

        return;
    }

    ret = ping_intf(PINT_DEST_IP, tc->priv, 0, 6,PING_TIMEOUT_MS);
    if (ret <= 0) {
        printf("ping %s failed\n",PINT_DEST_IP);
        uassert_true(false);
        return;
    }

    uassert_true(true);
}

int eth_tc_exit(struct utest_tc_export *tc)
{
    tc->priv = NULL;

    return 0;
}

static struct utest_tc_export g_eth0_test[] = {
    {
        .module = "eth",
        .name = "ping",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = eth0_tc_init,
        .tc         = eth_tc_test,
        .cleanup    = eth_tc_exit
    }
};

static struct utest_tc_export g_eth1_test[] = {
    {
        .module = "eth",
        .name = "ping",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = eth1_tc_init,
        .tc         = eth_tc_test,
        .cleanup    = eth_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_eth0_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_eth0_test[i].name);
    }
}

int func_eth0_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_eth0_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_eth0_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_eth0_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_eth0_test[i],loop_count);
    }

    return 0;
}

int func_eth1_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_eth1_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_eth1_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_eth1_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_eth1_test[i],loop_count);
    }

    return 0;
}
