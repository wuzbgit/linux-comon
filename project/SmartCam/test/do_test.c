#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>  

static int do_test()
{
    int level;
    int ret = 0;
    int i;

    ret = do_init();
    if (ret != 0) {
        printf ("do channel test fail:do init fail\n");

        return -1;
    }

    while (1)
    {
        level = 1;
        for (i = 0; i < 4; i++) {
            ret = do_out(i, level);
            if (ret < 0) {
                printf ("do channel %d test fail:do out error\n",i);

                return -1;
            }
            printf("do setting high\n");
        }
        usleep(3000 * 2000);

        level = 0;
        for (i = 0; i < 4; i++) {
            ret = do_out(i, level);
            if (ret < 0) {
                printf ("do channel %d test fail:do out error\n",i);

                return -1;
            }
            printf("do setting low\n");
        }        
        usleep(3000 * 2000);
    }    

    return 0;
}

static void display_usage()
{
    printf(
        "do_test  : do test\n"
        "Usage      : ./do_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --test,              -t         - do test\n"
        "    --set,           -s         - do set\n"
        "    --level,             -l         - set level(0 1)\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"test"        , no_argument, NULL, 't'},
    {"set"        , required_argument, NULL, 's'},
    {"level"        , required_argument, NULL, 'l'},
    {"help"          , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    int channel = -1;
    int ret = 0;
    int lvl = -1;
    int cmd = -1;

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"ts:l:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 't':
            cmd = 0;
            break;            
        case 's':
            cmd = 1;
            channel = atoi(optarg);
            break;
        case 'l':
            lvl = atoi(optarg);
            break;
        case 'c':
            channel = atoi(optarg);
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
        printf("invalid command or do channel\n");
        return -1;
    }
    
    if (cmd == 0) {
        ret = do_test();
    }
    else if (channel != -1) {
        if (cmd == 1) {
            if (( lvl != 0 ) && (lvl != 1)) {
                printf("invalid do level\n");
                return -1;
            }
        }

        do_init();
        
        do_out(channel, lvl);
    }

    return ret;
}
