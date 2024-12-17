#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>  
#include <linux/rtc.h>  

static void display_usage()
{
    printf(
        "rtc_test  : rtc test\n"
        "Usage      : ./rtc_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --init,          -i         - init rtc\n"
        "    --test,          -t         - test rtc\n"      
        "    --set,           -s         - set time\n"
        "    --get,           -g         - get time\n"
        "    --year,          -y         - set year\n"
        "    --month,         -m         - set month\n"
        "    --day,           -d         - set day\n"
        "    --hour,          -o         - set hour\n"
        "    --minuter,       -n         - set minuter\n"
        "    --second,        -c         - set second\n"                                        
        "\n"
    );
}

static struct option long_options[] = {
    {"init"        , no_argument, NULL, 'i'},
    {"test"        , no_argument, NULL, 't'},
    {"get"        , no_argument, NULL, 'g'},
    {"set"        , no_argument, NULL, 's'},
    {"year"        , required_argument, NULL, 'y'},
    {"month"        , required_argument, NULL, 'm'},
    {"day"        , required_argument, NULL, 'd'},
    {"hour"        , required_argument, NULL, 'o'},
    {"minuter"        , required_argument, NULL, 'n'},
    {"second"        , required_argument, NULL, 'c'},
    {"help"          , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    struct rtc_time tm;
    char buf[128] = {0};
    int ret = 0;
    int cmd = -1;
    int year = -1, mon = -1, day = -1, hour = -1, min = -1, sec = -1;

    if (argc == 1) {
        display_usage();

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"itsgy:m:d:o:n:c:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'i':
            cmd = 0;
            break;
        case 'g':
            cmd = 1;
            break;
        case 's':
            cmd = 2;
            break;
        case 't':
            cmd = 3;
            break;
        case 'y':
            year = atoi(optarg);
            break;
        case 'm':
            mon = atoi(optarg);
            break;
        case 'd':
            day = atoi(optarg);
            break;
        case 'o':
            hour = atoi(optarg);
            break;
        case 'n':
            min = atoi(optarg);
            break;
        case 'c':
            sec = atoi(optarg);
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
        exit(-1);
    }

    switch (cmd)
    {
    case 0:
        if (rtc_init() == 0) {
            printf("init rtc success\n");
        }
        break;
    case 1:
        ret = rtc_get(buf,128);
        if (ret == 0) {
            printf("get rtc time: %s\n",buf);
        }
        break;
    case 2:
        ret = read_rtc_time(&tm);
        if (ret == 0) {
            if (year > 1900) {
                tm.tm_year = year - 1900;
            }

            if (mon > 0) {
                tm.tm_mon = mon - 1;
            }

            if (day >= 0) {
                tm.tm_mday = day;
            }

            if (hour >= 0) {
                tm.tm_hour = hour;
            }

            if (min >= 0) {
                tm.tm_min = min;
            }

            if (sec >= 0) {
                tm.tm_sec = sec;
            }

            ret = rtc_set(tm);
            if (ret == 0) {
                rtc_to_ascii(tm, buf, 128);
                printf("set rtc time:%s\n",buf);
            }
        }
        else {
            perror("set rtc fail");
        }
        break;
    case 3:
        rtc_unit_test();
        break;
    default:
        ret = -1;
        break;
    }
    
    exit(ret);
}
