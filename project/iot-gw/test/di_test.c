#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>  
#include "gpio/gpio.h"

static void di_test()
{
    char level = 0;
    int ret = 0;
    int i;
    ret = di_init();
    if (ret != 0) {
        printf("di init fail\n");

        return ret;
    }
    
    printf("di testing....\n");
    while (1) {
        for (i = 0; i < 4; i++) {
            ret = di_read(i, &level);
            if (ret == 0) {
                printf("di channel %d level = %d\n",i,level);
            }            
        }
        usleep(1000 * 2000);
    }
}

static void display_usage()
{
    printf(
        "di_test  : di test\n"
        "Usage      : ./di_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --test,              -t         - di test\n"
        "    --get,               -g         - di get\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"test"        , required_argument, NULL, 't'},
    {"get"        , required_argument, NULL, 'g'},
    {"help"          , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int channel = -1;
    int cmd = -1;
    char level;
    int ret;

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"t:g:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 't':
            channel = atoi(optarg);
            cmd = 0;
        case 'g':
            channel = atoi(optarg);
            cmd = 1;
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }

    if (cmd == 0) {
        di_test(channel);  
    }
    else if (cmd == 1){
        ret = di_read(channel,&level);
        if (ret != 0) {
            printf("read ad channel %d fail\n",channel);
        }
        else {
            printf("ad channel %d value = %d\n",channel,level);
        }
    }

    return 0;
}
