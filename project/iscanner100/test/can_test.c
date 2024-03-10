#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
#include <getopt.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "utest/utest.h"

int can_fd = -1;
static int running = 1;

#define CAN_INTF_NAME              "can0"
#define CAN_READ_TIMEOUT_MS        30000
#define CAN_BIT_CLK                1000000
static void sigterm(int signo)
{
	running = 0;
}

static int set_can_clk(const char *intf,int clk)
{
    char buf[128] = {0};
    sprintf(buf,"ifconfig %s down",intf);
    system(buf);
    memset(buf,0,sizeof(buf));
    sprintf(buf,"ip link set %s up type can bitrate %d",intf,clk);
    system(buf);
}

int can_init(const char *intf)
{
	struct can_frame frame;
	struct ifreq ifr;
	struct sockaddr_can addr;
	char *intf_name = intf;
	int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
	int s; 

    set_can_clk(intf,CAN_BIT_CLK);

	if ((s = socket(family, type, proto)) < 0) {
		perror("socket");
		return -1;
	}

	addr.can_family = family;
    strcpy(ifr.ifr_name, intf_name);
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return -1;
	}

    return s;
}

int can_set_timeout(int fd,int timeout)
{
    struct timeval tm;
    int ret = 0;

    if(timeout > 0){
        tm.tv_sec  =  timeout/1000;
        tm.tv_usec = ((uint64_t)timeout-((uint64_t)timeout/1000)*1000)*1000;

        ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(struct timeval));

        if (ret < 0) return -1;
    }

    return 0;    
}

static int can_tc_init(struct utest_tc_export *tc)
{
    can_fd = can_init(CAN_INTF_NAME);

    return 0;
}

static void can_tc_test(struct utest_tc_export *tc)
{
    struct can_frame frame;
    int nbytes, i;
    int count = 0;

    if (can_fd <= 0) {
        uassert_true(false);

        return;
    }

    can_set_timeout(can_fd,CAN_READ_TIMEOUT_MS);

    while (count < 1000) {
        if ((nbytes = read(can_fd, &frame, sizeof(frame))) < 0) {
            perror("can read");

            uassert_true(false);

            return;
        }

        count++;
     	
        frame.can_id++;
		write(can_fd, &frame, sizeof(frame));   
    }

    uassert_true(true);
}

static void can_echo_tc_test(struct utest_tc_export *tc)
{
    struct can_frame frame;
    int nbytes, i;
    int count = 0;
    bool verbose = false;
    unsigned long last_timestamp,timestamp;
    struct timeval  tv;
    last_timestamp = timestamp = 0;

    if (can_fd <= 0) {
        uassert_true(false);

        return;
    }

    signal(SIGTERM, sigterm);
	signal(SIGHUP, sigterm);
	signal(SIGINT, sigterm);

    while (running) {
		if ((nbytes = read(can_fd, &frame, sizeof(frame))) < 0) {
			perror("can read");
             uassert_true(false);
			return ;
		}

        gettimeofday (&tv, NULL);
        timestamp= tv.tv_sec;
        if (last_timestamp + 1 < timestamp) {
            verbose = true;
            last_timestamp = timestamp;
        }
		if (verbose) {
            verbose = false;
			printf("%04x: ", frame.can_id);
			if (frame.can_id & CAN_RTR_FLAG) {
				printf("remote request");
			} else {
				printf("[%d]", frame.can_dlc);
				for (i = 0; i < frame.can_dlc; i++) {
					printf(" %02x", frame.data[i]);
				}
			}
			printf("\n");
		}
     	
        frame.can_id++;
		write(can_fd, &frame, sizeof(frame));   
    }

    uassert_true(true);
}

int can_tc_exit(struct utest_tc_export *tc)
{
    if (can_fd > 0) {
        close(can_fd);
    }

    return 0;
}

static struct utest_tc_export g_can_test[] = {
    {
        .module = "can",
        .name = "unit_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = can_tc_init,
        .tc         = can_tc_test,
        .cleanup    = can_tc_exit
    },
    {
        .module = "can",
        .name = "echo",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = can_tc_init,
        .tc         = can_echo_tc_test,
        .cleanup    = can_tc_exit
    }    
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_can_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_can_test[i].name);
    }
}

int func_can_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_can_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_can_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_can_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_can_test[i],loop_count);
    }

    return 0;
}
