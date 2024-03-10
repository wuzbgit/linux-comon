#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "utest/utest.h"

#define RTC_TIME_YEAR    2024
#define RTC_TIME_MON     03
#define RTC_TIME_DAY     11
#define RTC_TIME_HOUR    01
#define RTC_TIME_MIN     00
#define RTC_TIME_SEC     00

#define RTC_DEV_PATH    "dev/rtc"

static int rtc_write(int year,int month,int day,int hour,int minute,int second)
{
    int fd, retval;
    struct rtc_time rtc_tm;

    fd = open(RTC_DEV_PATH, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "open /dev/rtc error\n");
        return -1;
    }

    rtc_tm.tm_mday = year;
    rtc_tm.tm_mon = month;
    rtc_tm.tm_mday = day;
    rtc_tm.tm_hour = hour;
    rtc_tm.tm_min = minute;
    rtc_tm.tm_sec = second;

    retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
    if (retval == -1) {
        perror("ioctl");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static int rtc_read(int *year,int *month,int *day,int *hour,int *minute,int *second)
{
    int fd, retval;
    struct rtc_time rtc_tm;

    fd = open(RTC_DEV_PATH, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "open /dev/rtc error\n");
        return -1;
    }

    retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1) {
        perror("ioctl");
        return -1;
    }

    *year = rtc_tm.tm_mday;
    *month = rtc_tm.tm_mon;
    *day = rtc_tm.tm_mday;
    *hour = rtc_tm.tm_hour;
    *minute = rtc_tm.tm_min;
    *second = rtc_tm.tm_sec;

    close(fd);
    return 0;
}

static int rtc_tc_init(struct utest_tc_export *tc)
{
    if (access(RTC_DEV_PATH,F_OK)) {
        return -1;
    }

    return 0;
}

static void rtc_tc_test(struct utest_tc_export *tc)
{
    int year = 0,mon = 0,day = 0,hour = 0,min = 0,sec = 0;
    int compare = 0;
    bool status = false;

    if (access("/data/rtc_test",F_OK) == 0) {
        if (access("/tmp/rtc_test",F_OK)) {
            compare = 1;
            if (rtc_read(&year,&mon,&day,&hour,&min,&sec) == 0) {
                if (year == RTC_TIME_YEAR) {
                    status = true;
                }
            }
        }
    }
    else {
        if (rtc_write(RTC_TIME_YEAR,RTC_TIME_MON,RTC_TIME_DAY,RTC_TIME_HOUR,RTC_TIME_MIN,RTC_TIME_SEC) != 0) {
            uassert_true(false);
        }
        else {
            system("echo \"rtc\" > /data/rtc_test");
            system("echo \"rtc\" > /tmp/rtc_test");
        }
    }
    
    if (compare == 0) {
        printf("rtc test case need reset power\n");
    }
    else if ((compare == 1) && (status == true)) {
        uassert_true(true);
    }
    else {
       uassert_true(false); 
    }  
}

int rtc_tc_exit(struct utest_tc_export *tc)
{
    return 0;
}

static struct utest_tc_export g_rtc_test[] = {
    {
        .module = "rtc",
        .name = "unit_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = rtc_tc_init,
        .tc         = rtc_tc_test,
        .cleanup    = rtc_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_rtc_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_rtc_test[i].name);
    }
}

int func_rtc_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_rtc_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_rtc_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_rtc_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_rtc_test[i],loop_count);
    }

    return 0;
}
