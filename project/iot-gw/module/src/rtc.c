#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>  
#include <linux/rtc.h>  

#define RTC_DEV "/dev/rtc0"

void rtc_to_ascii(struct rtc_time tm, char *buf, unsigned bufsize)
{
    snprintf(buf, bufsize - 1, "%d-%02d-%02d %02d:%02d:%02d",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,  
           tm.tm_hour, tm.tm_min, tm.tm_sec);    
}

int read_rtc_time(struct rtc_time *rtc_tm)
{  
    int fd;  
      
    fd = open(RTC_DEV, O_RDONLY);  
    if (fd == -1) {  
        return -1;  
    }  

    if (ioctl(fd, RTC_RD_TIME, rtc_tm) == -1) {  
        close(fd);  
        return -1;  
    }  

    close(fd);  
    return 0;  
}

int rtc_get(char *buf,unsigned bufsize)
{
    int rtc_fd;  
    struct rtc_time tm;
    int ret = 0;

    ret = read_rtc_time(&tm);
    if (ret < 0) {
        perror("read rtc faile");

        return -1;
    }

    rtc_to_ascii(tm, buf, bufsize);
    
    close(rtc_fd);

    return 0;
}

int rtc_set(struct rtc_time tm)
{
    int rtc_fd;
    int ret = 0;
    
    rtc_fd = open(RTC_DEV, O_RDWR);  
    if (rtc_fd < 0) {  
        perror("Error opening /dev/rtc0");  
        return -1;  
    }  

    ret = ioctl(rtc_fd, RTC_SET_TIME, &tm);
    if (ret < 0) {  
        perror("Error reading RTC time");  
        close(rtc_fd);  
        return -1;  
    }

    close(rtc_fd);

    return 0;
}

int rtc_init()
{
    int ret = 0;
    ret = system("date -s \"2024-11-13 14:20:00\" && hwclock -w -f /dev/rtc0");
    if (ret != 0) {
        printf("rtc test fail\n");

        return -1;
    }

    return 0;
}

int rtc_unit_test()
{
    int ret;
    char buf[128] = {0};
    struct rtc_time tm;
    int state = -1;

    ret = rtc_init();
    if (ret == 0) {
        ret = read_rtc_time(&tm);
        if (ret == 0) {
            if ((tm.tm_year + 1900 == 2024) && (tm.tm_mon+1 == 11) && (tm.tm_mday == 13)) {
                state = 0;
            }
        }
    }

    return state;
}