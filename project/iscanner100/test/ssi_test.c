#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ssi.h"
#include "utest/utest.h"

#define SSI_READ_DELAY_US 500000
#define SSI_CLK           1000

static bool running = true;
static int fd_ssi = -1;

static void sigterm(int signo)
{
	running = false;
}

static int ssi_tc_init(struct utest_tc_export *tc)
{
    fd_ssi = ssi_init(SSI_CLK);
    if (fd_ssi <= 0) {
        printf("intialize ssi failed,try again\r\n");

        return -1;
    }
    return 0;
}

static void ssi_tc_test(struct utest_tc_export *tc)
{
    unsigned char buf[4];
    unsigned ssi_data = 0;
    unsigned clk;
    unsigned encoderValue = 0;
    int ret = 0;
    int count = 0;

    if (fd_ssi <= 0) {
        uassert_true(false);
        return;        
    }

    signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	signal(SIGINT, sigterm);
    do
    {
        memset(buf, 0, sizeof(buf));

        ret = ssi_read(fd_ssi, buf, 4, 0);
        if (ret <= 0) {
            printf("ssi read failed\r\n");
            uassert_true(false);
            return;
        }
        else {
            ssi_data = (buf[1] << 8) | (buf[0]);
            encoderValue = ssi_data;
            while (ssi_data >>= 1) {
                encoderValue ^= ssi_data;
            }

            count++;

            printf("encoderValue=%d\r\n", encoderValue);
        }
        usleep(SSI_READ_DELAY_US);

        if (count > 100) {
            break;
        }
    } while (running);

    if (count > 100) {
        uassert_true(true);
    }
    else {
        uassert_true(false);
    }      
}

static void ssi_tc_read(struct utest_tc_export *tc)
{
    unsigned char buf[4];
    unsigned ssi_data = 0;
    unsigned clk;
    unsigned encoderValue = 0;
    int ret = 0;
    int count = 0;

    if (fd_ssi <= 0) {
        uassert_true(false);
        return;        
    }

    signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	signal(SIGINT, sigterm);

    do
    {
        memset(buf, 0, sizeof(buf));

        ret = ssi_read(fd_ssi, buf, 4, 0);
        if (ret <= 0) {
            printf("ssi read failed\r\n");
            uassert_true(false);
            return;
        }
        else {
            ssi_data = (buf[1] << 8) | (buf[0]);
            encoderValue = ssi_data;
            while (ssi_data >>= 1) {
                encoderValue ^= ssi_data;
            }

            count++;
            printf("encoderValue=%d\r\n", encoderValue);
        }
        usleep(SSI_READ_DELAY_US);

    } while (running);

    if (count > 0) {
        uassert_true(true);
    }
    else {
        uassert_true(false);
    }  
}

int ssi_tc_exit(struct utest_tc_export *tc)
{
    if (fd_ssi > 0) {
        ssi_deinit(fd_ssi);
    }
    
    return 0;
}

static struct utest_tc_export g_ssi_test[] = {
    {
        .module = "ssi",
        .name = "unit_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = ssi_tc_init,
        .tc         = ssi_tc_test,
        .cleanup    = ssi_tc_exit
    },
    {
        .module = "ssi",
        .name = "read",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = ssi_tc_init,
        .tc         = ssi_tc_test,
        .cleanup    = ssi_tc_exit
    }    
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_ssi_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_ssi_test[i].name);
    }
}

int func_ssi_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_ssi_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_ssi_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_ssi_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_ssi_test[i],loop_count);
    }

    return 0;
}
