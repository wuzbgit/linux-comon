#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#define LOG_DIR "/var/log"
#define LOG_FILE LOG_DIR "/sdcard.log"

static pthread_mutex_t log_mutex;
static FILE *log_file = NULL;
#define MAX_LOG_SIZE (10 * 1024 * 1024)

static void get_time_str(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

static void check_log_size()
{
    struct stat st;
    if (stat(LOG_FILE, &st) == 0 && st.st_size >= MAX_LOG_SIZE) {
        log_file = fopen(LOG_FILE, "w");
        if (log_file) {
            fprintf(log_file, "[LOG RESET] Log file exceeded 10MB, resetting...\n");
            fclose(log_file);
        }
    }
}

void sd_log_init()
{
    pthread_mutex_init(&log_mutex, NULL);
    if (log_file != NULL) {
        return;
    }

    struct stat st;
    if (stat(LOG_DIR, &st) == -1) {
        if (mkdir(LOG_DIR, 0755) == -1) {
            perror("Failed to create log directory");
            return;
        }
    }

    log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Failed to create/open log file");
        return;
    }

    fclose(log_file);

    check_log_size();
}

void sd_log(const char *format, ...)
{
    char time_buf[20];

    pthread_mutex_lock(&log_mutex);

    check_log_size();
    
    log_file = fopen(LOG_FILE, "a");
    if (!log_file) {
        perror("Failed to open log file");
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    get_time_str(time_buf, sizeof(time_buf));
    fprintf(log_file, "[%s] ", time_buf);

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fprintf(log_file, "\n");
    fclose(log_file);

    pthread_mutex_unlock(&log_mutex);
}

void sd_log_close()
{
    pthread_mutex_destroy(&log_mutex);
}
