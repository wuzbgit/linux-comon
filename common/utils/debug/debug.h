#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} log_level_t;

#undef LOG_DEBUG
#undef LOG_TRACE
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#define LOG_DEBUG(fmt, args...)      print_level(LOG_LEVEL_DEBUG,fmt,##args)
#define LOG_INFO(fmt, args...)       print_level(LOG_LEVEL_INFO,fmt,##args)
#define LOG_NOTICE(fmt, args...)     print_level(LOG_LEVEL_NOTICE,fmt,##args)
#define LOG_WARN(fmt, args...)       print_level(LOG_LEVEL_WARN,fmt,##args)
#define LOG_ERROR(fmt, args...)      print_level(LOG_LEVEL_ERROR,fmt,##args)
#define LOG_FATAL(fmt, args...)      print_level(LOG_LEVEL_FATAL,fmt,##args)

#ifdef __cplusplus
}
#endif

void print_level(int level,const char *fmt, ...);
#endif