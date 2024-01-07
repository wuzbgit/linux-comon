#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utest/utest.h"
#include "im3332.h"

static void *handle = NULL;

static int im3332_tc_init(struct utest_tc_export *tc)
{
    handle = im3332_init();
    if (!handle) {
        return -1;
    }

    return 0;
}

static void im3332_tc_test(struct utest_tc_export *tc)
{
    uint16_t version,model;
    if (handle == NULL) {
        uassert_true(false);
        return;
    }

    if (im3331_set_mode(handle,MODE_THREEPHASE_FOURWIRE) != 0) {
        uassert_true(false);
        return;
    }

    if (im3332_get_sw_version(handle,&model) != 0) {
        uassert_true(false);
        return;
    }

    if (im3332_get_sw_version(handle,&version) != 0) {
        uassert_true(false);
        return;
    }

    if (model != 2) {
        uassert_true(false);
        return;        
    }

    uassert_true(true);   
}

int im3332_tc_exit(struct utest_tc_export *tc)
{
    im3332_deinit(handle);

    return 0;
}

static char *buf = NULL;
static int im3332_sample_tc_init(struct utest_tc_export *tc)
{
    handle = im3332_init();
    if (im3332_sample_init(handle) != 0) {

        return -1;
    }
    
    buf = (char*)malloc(8192);   
    if (buf == NULL) {
        im3332_deinit(handle);
    }

    im3332_sample_enable(handle);
    return 0;
}

static void im3332_sample_tc_test(struct utest_tc_export *tc)
{
    int len;
    len = im3332_sample_read(buf,8192);
    if (len <= 0) {
        uassert_true(false);
        return;
    }

    uassert_true(true);    
}

int im3332_sample_tc_exit(struct utest_tc_export *tc)
{
    im3332_sample_disable(handle);
    im3332_deinit(handle);
    free(buf);

    return 0;
}
static struct utest_tc_export g_im3332_test[] = {
    {
        .module = "im3332",
        .name = "module_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = im3332_tc_init,
        .tc         = im3332_tc_test,
        .cleanup    = im3332_tc_exit
    },
    {
        .module = "im3332",
        .name = "sample_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = im3332_sample_tc_init,
        .tc         = im3332_sample_tc_test,
        .cleanup    = im3332_sample_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_im3332_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_im3332_test[i].name);
    }
}

int func_im3332_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_im3332_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_im3332_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_im3332_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_im3332_test[i],loop_count);
    }

    return 0;
}


