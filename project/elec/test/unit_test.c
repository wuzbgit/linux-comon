#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include "utest/utest.h"

#define MODULE_MAX_NUM  50

typedef int (*module_func)(const char *case_name,unsigned loop_count,int argc,char *argv[]);
struct elec_test
{
    char *module_name;
    module_func module_process;
};

extern int func_im3332_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[]);
extern int func_rf_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[]);
extern int func_eth_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[]);
extern int func_led_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[]);

static struct elec_test g_elec_test[MODULE_MAX_NUM] = {
    {
        .module_name = "led",
        .module_process = func_led_test_main,       
    },    
    {
        .module_name = "im3332",
        .module_process = func_im3332_test_main,
    },
    {
        .module_name = "rf",
        .module_process = func_rf_test_main,       
    },
    {
        .module_name = "eth",
        .module_process = func_eth_test_main,       
    }     
};

static struct option long_options[] = {
    {"module"        , required_argument, NULL, 'm'},
    {"case"          , required_argument, NULL, 'c'},
    {"loop_cout"     , required_argument, NULL, 'l'},
    {"file"          , required_argument, NULL, 'f'},
    {"help"          , required_argument, NULL, 'h'}
};

void display_common_usage()
{
    printf(
        "elec_test  : electric test\n"
        "Usage      : elec_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --module,           -m         - Test module\n"
        "    --case,             -c         - Test case\n"
        "    --loop_count        -l         - Loop times\n"
        "    --file              -f         - Report file\n"
        "\n"
    );
}

static void display_module_usage()
{
    int i = 0;
    int module_cnt = sizeof(g_elec_test)/sizeof(g_elec_test[0]);

    display_common_usage();

    printf("avail module list:\n");
    for (i=0;i<module_cnt;i++) {
        if (NULL != g_elec_test[i].module_name) {
            printf("            %s\n",g_elec_test[i].module_name);
        }
    }
}

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    char case_name[100] = {0};
    char *report_file = NULL;
    char *module_name = NULL;
    unsigned loop_count = 1;
    int ret;
    unsigned module_count = 0;
    unsigned i,j;
    char msg[120] = {0};
    int status = 0;

    while((opt = getopt_long(argc,argv,"m:c:l:f:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'm':
            module_name = optarg;
            break;
        case 'c':
            strcpy(case_name,(const char*)optarg);
            break;
        case 'f':
            report_file = optarg;
            break;
        case 'l':
            loop_count = atoi(optarg);
            break;
        case 'h':
        case '?':
            break;
        default:
            break;
        }
    }

    if (!module_name) {
        display_module_usage();
        return -1;
    }

    ret = sprintf(msg,"Command:");
    for (i=0;i<argc;i++) {
        ret += sprintf(msg+ret,"%s ",argv[i]);
    }

    ret = utest_init(report_file,msg);
    if (ret != 0) {
        printf("utest init fail\n");

        return -1;
    }

    module_count = sizeof(g_elec_test)/sizeof(g_elec_test[0]);
    if (!strcmp(module_name,"all")) {
        for (i=0;i<module_count;i++) {
            if (NULL != g_elec_test[i].module_process) {
                if (g_elec_test[i].module_process("all",loop_count,argc,argv) == 0) {
                    status |= 1<<i;
                }
            }
        }
    }
    else {
        for (i=0;i<module_count;i++) {
            if (!strcmp(module_name,g_elec_test[i].module_name)) {
                if (g_elec_test[i].module_process(case_name,loop_count,argc,argv) == 0) {
                    status |= 1<<i;
                }
                break;
            }
        }

        if (i == module_count) {
            display_module_usage();
            return -1;
        }
    }

    if (status) {
        utest_statistic();
    }

    utest_deinit();

    return 0;
}
