#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>  
#include "mvcam.h"
#include "strfunc.h"

static void display_usage()
{
    printf(
        "sensor_test  : sensor test\n"
        "Usage      : ./sensor_test -r reg\n"
        "Usage      : ./sensor_test -w reg -v value\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --read,          -r         - read register\n"
        "    --write,         -w         - write register\n"
        "    --value,         -v         - write register value\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"read"        , required_argument, NULL, 'r'},
    {"write"        , required_argument, NULL, 'w'},
    {"value"        , required_argument, NULL, 'v'},
    {"help"          , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int ret = 0;
    int cmd = -1;
    unsigned reg;
    int value = 0xFFFFFF;

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"r:w:v:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'r':
            cmd = 0;
            if (StrToNumber(optarg , (unsigned *)&reg) != 0) {
                printf("invalid register %s format\n", optarg);
                exit(1);
            }
            break;            
        case 'w':
            cmd = 1;
            if (StrToNumber(optarg , (unsigned *)&reg) != 0) {
                printf("invalid register %s format\n", optarg);
                exit(1);
            }
            break;
        case 'v':
            if (StrToNumber(optarg , (unsigned *)&value) != 0) {
                printf("invalid value %s format\n", optarg);
                exit(1);
            }        
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
        printf("invalid command\n");
        return -1;
    }

    if (cmd == 0) {
        ret = mvcam_read_reg(reg, &value);
        if (ret == 0) {
            printf("sensor reg[0x%x] is 0x%x\n",reg,value);
        }
        else {
            printf("read mvcam reg[0x%x] failed\n",reg);
        }
    }
    else if (cmd == 1) {
        if (value != 0xFFFFFF) {
            ret = mvcam_write_reg(reg,value);
            if (ret == 0) {
                printf("sensor write reg[0x%x] 0x%x success\n",reg,value);
            }
            else {
                printf("sensor write reg[0x%x] 0x%x failed\n",reg,value);
            }            
        }
        else {
            printf("invalid write value\n");
        }       
    }

    return ret;
}
