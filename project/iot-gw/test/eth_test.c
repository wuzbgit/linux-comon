#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include "network/netip.h"
#include "network/ping.h"

#define PING_TIMEOUT_MS 500

static int eth_test(char *intf,char *ip)
{
    int ret;
    if (netif_exist(intf) == false) {
        return -1;
    }

    ret = ping_intf(ip, intf, 0, 6, PING_TIMEOUT_MS);
    if (ret <= 0) {
        return -1;
    }
    
    return 0;
}

static void display_usage()
{
    printf(
        "eth_test  : eth test\n"
        "Usage      : ./eth_test [options]\n"
        "\n"
        "Options [default in brackets]:\n"
        "    --eth,           -e         - eth interface\n"
        "    --ip,            -i         - destination ip\n"
        "\n"
    );
}

static struct option long_options[] = {
    {"eth"        , required_argument, NULL, 'e'},
    {"ip"        , required_argument, NULL, 'i'},
    {"help"          , no_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    char *intf = NULL;
    char *ip = NULL;
    int ret = -1;

    if (argc == 1) {
        display_usage();

        return -1;
    }

    while((opt = getopt_long(argc,argv,"e:i:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'e':
            intf = optarg;
            break;
        case 'i':
            ip = optarg;
            break;
        case 'h':
        case '?':
            display_usage();
            break;
        default:
            break;
        }
    }

    if ((ip == NULL) || (intf == NULL)) {
        display_usage();

        return -1;
    }

    ret = eth_test(intf, ip);
    if (ret != 0) {
        printf("eth test fail: ping %s failed\n",ip);
    }
    else {
        printf("%s test succss\n",intf);
    }
    return ret;
}
