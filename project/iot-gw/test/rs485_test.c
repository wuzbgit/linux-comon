#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <errno.h>  
#include <termios.h>  
#include <linux/serial.h>

extern int rs485_echo_test(char *devpath);

static void display_usage()
{
    printf(
        "rs485  : rs485 test\n"
        "Usage      : rs485 echo test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --device,           -d         - Test device\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"device"        , required_argument, NULL, 'd'},
    {"help"          , required_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    char devpath[100] = {0};

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"d:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'd':
            strncpy(devpath,optarg,sizeof(devpath) - 1);
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }

    if (strlen(devpath) == 0) {
        display_usage();

        return 0;
    }

    printf("starting rs485 %s echo test\n",devpath);

    rs485_echo_test(devpath);

    return 0;
}
