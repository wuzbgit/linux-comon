#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include "rf/rf_common.h"

static int rf_module_print()
{
    int ret = 0;
    RFINFO_T RF_info;

    ret = rf_module_info(&RF_info);
    if (ret != 0) {
        printf("rf module print fail:can not get rf module info\n");

        return -1;
    }

    rf_info_print(&RF_info);

    return 0;
}

static int rf_unit_test()
{
    int ret;
    printf("rf testing...\n");
    ret = rf_reset_test();
    if (ret != 0) {
        printf("rf reset test fail\n");
    }
    else {
        printf("rf reset test success\n"); 
    }

    ret = rf_module_test();
    if (ret != 0) {
        printf("rf module test fail:invalid rf module\n");
    }
    else {
        printf("rf module test succss\n");
    }

    ret = rf_simdetect_test();
    if (ret != 0) {
       printf("rf sim card detect test fail\n"); 
    }
    else {
        printf("rf sim card detect test success\n");
    }

    return ret;
}

static void display_usage()
{
    printf(
        "rf_test  : rf test\n"
        "Usage      : ./rf_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --reset,           -r         - rf module reset test\n"
        "    --module,          -m         - rf module test\n"
        "    --info,            -i         - print rf module info\n"
        "    --sim,             -s         - state enable(detect) or disale(detect)\n"
        "    --dial,            -d         - rf module dial\n"
        "    --net,             -n         - rf module net test\n"
        "    --help\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"reset"        , no_argument, NULL, 'r'},
    {"module"       , no_argument, NULL, 'm'},
    {"info"         , no_argument, NULL, 'i'},
    {"dial"         , no_argument, NULL, 'd'},
    {"net"          , no_argument, NULL, 'n'},
    {"sim"          , required_argument, NULL, 's'},
    {"test"          ,no_argument, NULL, 't'},    
    {"help"         , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int ret = -1;

    if (argc == 1) {
        display_usage();

        return -1;
    }

    while((opt = getopt_long(argc,argv,"rmits:dnh?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'r':
            ret = rf_reset_test();
            if (ret != 0) {
                printf("rf reset test fail\n");
            }
            else {
               printf("rf reset test success\n"); 
            }
            break;
        case 'm':
            ret = rf_module_test();
            if (ret != 0) {
                printf("rf module test fail:invalid rf module\n");
            }
            else {
                printf("rf module test succss\n");
            }
            break;
        case 'i':
            ret = rf_module_print();
            break;
        case 's':
            if (strcmp(optarg,"enable") == 0) {
                ret = rf_module_sim_detect_enable(1);
            }
            else if (strcmp(optarg,"disable") == 0) {
                ret = rf_module_sim_detect_enable(0);
            }
            break;
        case 'd':
            ret = rf_module_dial();
            break;
        case 'n':
            ret = rf_net_test();
            break;
        case 't':
            ret = rf_unit_test();
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }
    
    return ret;
}
