#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "debug.h"

static log_level_t log_level = LOG_LEVEL_INFO;

static char *g_debug_label[] = {
    "[fatal]",
    "[error]",
    "[warn]",
    "[notice]",
    "[info]",
    "[debug]"
};

const char * get_time(void) {
    static char time_buf[50];
    struct timeval  tv;
    time_t time;
    suseconds_t millitm;
    struct tm *ti;

    gettimeofday (&tv, NULL);

    time= tv.tv_sec;
    millitm = (tv.tv_usec + 500) / 1000;

    if (millitm == 1000) {
        ++time;
        millitm = 0;
    }

    ti = localtime(&time);
    sprintf(time_buf, "[%02d-%02d_%02d:%02d:%02d:%03d]", ti->tm_mon+1, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec, (int)millitm);
    return time_buf;
}

static void __print_level(const char *label,const char *fmt,va_list arg)
{
    char *buf = NULL;

    buf = get_time();

    if (label) 
        printf("%s %s",label,buf);
    else 
        printf("%s",buf);
        
    vprintf(fmt,arg);
}

void print_level(int level,const char *fmt, ...)
{
    va_list arg;

    if (level > LOG_LEVEL_DEBUG) {
        level = LOG_LEVEL_DEBUG;
    }

    va_start(arg,fmt);
    if (log_level >= level) {
        __print_level(g_debug_label[level],fmt,arg);
    }
    va_end(arg);
}