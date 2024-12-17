#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>

static void ai_test()
{
    int ret = 0;
    int value = 0;
    int i;
    printf("ai testing....\n");
    while (1) {

        for (i = 0; i < 2; i++) {
            ret = ai_read_channel(i, &value);
            if (ret == 0) {
                printf("AI Channel %d is : %dmV %fmA\n", i, value,value*7.414/1000);
            }
            else {
                printf("AI Channel %d fail\n", i);
            }
        }

        usleep(1000 * 2000);
    }
}

static void display_usage()
{
    printf(
        "ai_test  : ai test\n"
        "Usage      : ./ai_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --test,               -t         - ai test\n"
        "    --get,               -g         - ai get\n"
        "    --channel,           -c         - ai channel\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"test"        , no_argument, NULL, 't'},
    {"get"        , no_argument, NULL, 'g'},
    {"channel"        , required_argument, NULL, 'c'},
    {"help"          , required_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int channel = -1;
    int cmd = -1;

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"c:d:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'c':
            channel = atoi(optarg);
            break;
        case 't':
            cmd = 0;
            break;
        case 'g':
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

    if (cmd == -1) {
        display_usage();
        return 0;
    }

    if (cmd == 0) {
        ai_test();
    }
    else if(cmd == 1 && channel != -1) {
        int ret;
        int value = 0;
        ret = ai_read_channel(channel, &value);
        if (ret != 0) {
            printf("read ad channel %d fail\n",channel);
        }
        else {
            printf("ad channel %d value = %d\n",channel,value);
        }
    }
    else {
        display_usage();
        return 0;        
    }
    return 0;
}
