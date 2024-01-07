#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "utest.h"

static struct utest local_utest = {UTEST_PASSED, 0, 0};
static FILE *g_report_fp = NULL;
static struct utest_tc_export *g_tc = NULL;

static const char *file_basename(const char *file)
{
    char *end_ptr = NULL;
    char *rst = NULL;

    if (!((end_ptr = strrchr(file, '\\')) != NULL || \
        (end_ptr = strrchr(file, '/')) != NULL) || \
        (strlen(file) < 2))
    {
        rst = (char *)file;
    }
    else
    {
        rst = (char *)(end_ptr + 1);
    }
    return (const char *)rst;
}

utest_t utest_handle_get()
{
    return (utest_t)&local_utest;
}

void utest_assert(int value, const char *file, int line, const char *func, const char *msg)
{
    if (!(value))
    {
        local_utest.error = UTEST_FAILED;
        local_utest.failed_num ++;
        printf("[  ASSERT   ] [ %s %s ] at (%s); func: (%s:%d); msg: (%s) failed\n",g_tc->module,g_tc->name,file_basename(file), func, line, msg);
        if (g_report_fp) {
            fprintf(g_report_fp,"[  ASSERT   ] [ %s %s ] at (%s); func: (%s:%d); msg: (%s)",g_tc->module,g_tc->name,file_basename(file), func, line, msg);
            fflush(g_report_fp);
        }
    }
    else
    {
        printf("[    OK     ] [ %s %s ] (%s:%d) is passed\n", g_tc->module,g_tc->name,func, line);
        if (g_report_fp) {
            fprintf(g_report_fp,"[    OK     ] [ %s %s ] (%s:%d) is passed", g_tc->module,g_tc->name,func, line);
            fflush(g_report_fp);
        }
        local_utest.error = UTEST_PASSED;
        local_utest.passed_num ++;
    }
}

void utest_assert_string(const char *a, const char *b, bool equal, const char *file, int line, const char *func, const char *msg)
{
    if (a == NULL || b == NULL) {
        utest_assert(0, file, line, func, msg);
    }

    if (equal) {
        if (strcmp(a, b) == 0) {
            utest_assert(1, file, line, func, msg);
        }
        else {
            utest_assert(0, file, line, func, msg);
        }
    }
    else {
        if (strcmp(a, b) == 0) {
            utest_assert(0, file, line, func, msg);
        }
        else {
            utest_assert(1, file, line, func, msg);
        }
    }
}

void utest_assert_buf(const char *a, const char *b, size_t sz, bool equal, const char *file, int line, const char *func, const char *msg)
{
    if (a == NULL || b == NULL) {
        utest_assert(0, file, line, func, msg);
    }

    if (equal) {
        if (memcmp(a, b, sz) == 0) {
            utest_assert(1, file, line, func, msg);
        }
        else {
            utest_assert(0, file, line, func, msg);
        }
    }
    else {
        if (memcmp(a, b, sz) == 0) {
            utest_assert(0, file, line, func, msg);
        }
        else {
            utest_assert(1, file, line, func, msg);
        }
    }
}

int do_test(struct utest_tc_export *tc,unsigned loop_count)
{
    int ret = 0;
    unsigned loop = 0;
    do {
        loop++;

        if (tc->tc) {
            printf("\n[    RUN    ] [ %s %s ]\n",g_tc->module,g_tc->name);
            tc->tc(tc);
        }
    }while(loop < loop_count);
}

int waitpid_timeout(pid_t pid,int mseconds,int *status)
{
    sigset_t mask,orig_mask;
    struct timespec timeout;
    int ret = -1;
    pid_t w;

    if (mseconds <= 0) {
        do {
            w = waitpid(pid, status, 0);
            if (w == -1) {
                return -1;
            }
        } while(!WIFEXITED(*status) && !WIFSIGNALED(*status));

        ret = 0;
    }
    else {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);

        if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
            perror ("sigprocmask");
            ret = 0;
        }
        else {
            timeout.tv_sec = mseconds/1000;
            timeout.tv_nsec = (mseconds%1000) * 1000000;
            do {
                if (sigtimedwait(&mask,NULL,&timeout) < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    else if (errno == EAGAIN) {
                        kill(pid,SIGKILL);
                    }
                    else {
                        perror("sigtimedwait:");
                    }
                }
                else {
                    int wait_status;
                    pid_t child_pid = waitpid(pid,&wait_status,WNOHANG);
                    if (child_pid == pid) {
                        if (WIFEXITED(wait_status) || WIFSIGNALED(wait_status)) {
                            ret = 0;
                        }
                    }
                    else {
                        continue;
                    }
                }
                break;
            }while(1);

            if(waitpid(pid,NULL,0) < 0) {
                perror("waitpid");
            }
        }
    }

    return ret;
}

int utest_run(struct utest_tc_export *tc,unsigned loop_count)
{
    int pid;
    int ret,status;

    g_tc = tc;
    if(tc->init) {
        ret = tc->init(tc);
        if(ret != 0) {
            uassert_true(false);
            return -1;
        }
    }

    if (tc->process_fork == true) {
        pid = fork();
        if (pid == 0) {
            do_test(tc,loop_count);
            exit(0);
        }
        else {
            ret = waitpid_timeout(pid,tc->run_timeout,&status);
            if (tc->fork_assert) {
                tc->fork_assert(tc,ret,status);
            }
            else {
                if (ret == 0) {
                    uassert_true(true);
                }
                else {
                    uassert_true(false);
                }
            }
        }
    }
    else {
        do_test(tc,loop_count);
    }

    if (tc->cleanup) {
        tc->cleanup(tc);
    }

    return 0;
}

int utest_init(const char *report_path,const char *msg)
{
    struct timeval tv;
    struct timezone tz;
    struct tm       *p;
    char buf[80];
    local_utest.error = UTEST_PASSED;
    local_utest.passed_num = 0;
    local_utest.failed_num = 0;

    if (report_path) {
        g_report_fp = fopen(report_path,"w");
        if (g_report_fp == NULL) {
            printf("can not open file:%s\n",report_path);
            return -1;
        }

        if (msg) {
            fprintf(g_report_fp,"%s\n",msg);
        }

        gettimeofday(&tv,&tz);
        p = localtime(&tv.tv_sec);
        sprintf(buf,"Time:%4d-%02d-%02d %2d:%02d:%02d.%ld",1900+p->tm_year,1+p->tm_mon,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,tv.tv_usec);
        fprintf(g_report_fp,"%s\n\n",buf);
        fflush(g_report_fp);
    }
    
    return 0;
}

int utest_statistic()
{
    printf("---------test case statictics:----------\n");
    printf("---------passed number:%d----------\n",local_utest.passed_num);
    printf("---------failed number:%d----------\n",local_utest.failed_num);

    if (g_report_fp) {
        fprintf(g_report_fp,"---------test case statictics:----------\n");
        fprintf(g_report_fp,"---------passed number:%d----------\n",local_utest.passed_num);
        fprintf(g_report_fp,"---------failed number:%d----------\n",local_utest.failed_num);        
    }
}

void utest_deinit()
{
    if (g_report_fp) {
        fclose(g_report_fp);
    }
}