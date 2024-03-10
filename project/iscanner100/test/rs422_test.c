#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "utest/utest.h"

#define RS422_PATH      "/dev/ttymxc1"
// 串口通用初始化函数
static int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio; // 定义结构体newtio和oldtio
    // 将原串口的数据取到oldtio
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    // 将newio清零和设置c_cflag
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD; // 使能接收和忽略控制线
    newtio.c_cflag &= ~CSIZE;
    // 设置数据位
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    // 设置校验位
    switch (nEvent)
    {
    // 偶校验
    case 'O':
        newtio.c_cflag |= PARENB;           // 使能奇偶校验
        newtio.c_cflag |= PARODD;           // 偶校验
        newtio.c_iflag |= (INPCK | ISTRIP); // 输入校验并忽略第八位
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD; // 取消偶校验（置零偶校验位），开启奇校验
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB; // 不进行奇偶校验
        break;
    }
    // 设置波特率
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    // 设置停止位
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB; // 一位停止位
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB; // 两位停止位

    newtio.c_cc[VTIME] = 0; // 不设置读取超时
    newtio.c_cc[VMIN] = 1;  // 读取最小字符数为0

    tcflush(fd, TCIFLUSH); // 清空缓冲区

    // 使配置生效
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    //	printf("set done!\n\r");
    return 0;
}

int set_serial_timeouts(int fd, int timeout) {
    struct termios tty;
 
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }
 
    // 设置VMIN为0，表示无数据不阻塞
    tty.c_cc[VMIN] = 0;
    // 设置VTIME为read_timeout秒，0表示1秒
    tty.c_cc[VTIME] = timeout;
 
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
 
    return 0;
}

static int rs422_tc_init(struct utest_tc_export *tc)
{

    return 0;
}

static void rs422_tc_test(struct utest_tc_export *tc)
{
    int fd, nByte;
    char *uart3;
    char bufferR[256];
    char bufferW[256];
    char *test_str = "please enter some charcters\n";

    memset(bufferR, '\0', sizeof(bufferR));
    memset(bufferW, '\0', sizeof(bufferW));

    if ((fd = open(RS422_PATH, O_RDWR, 0777)) < 0) {
        printf("failed open device=%s\n", RS422_PATH);
        uassert_true(false);
        return 0;
    }
    else {
        set_opt(fd, 115200, 8, 'N', 1);
        set_serial_timeouts(fd,5);
    }

    write(fd, test_str, strlen(test_str));
    nByte = read(fd, bufferR, sizeof(bufferR) - 1);
    if((nByte > 0) && (strncmp(bufferW,test_str,strlen(test_str)) == 0)) {
        uassert_true(true);
    }
    
    close(fd);

    uassert_true(false);
}

int rs422_tc_exit(struct utest_tc_export *tc)
{
    return 0;
}

static struct utest_tc_export g_rs422_test[] = {
    {
        .module = "led",
        .name = "unit_test",
        .run_timeout = 5000,
        .process_fork = false,
        .fork_assert = NULL,
        .init       = rs422_tc_init,
        .tc         = rs422_tc_test,
        .cleanup    = rs422_tc_exit
    }
};
extern void display_common_usage();
static void display_usage()
{
    int i;
    int items = sizeof(g_rs422_test)/sizeof(struct utest_tc_export);
    display_common_usage();
    printf("case list\n");
    for (i=0;i<items;i++) {
        printf("     %s\n",g_rs422_test[i].name);
    }
}

int func_rs422_test_main(const char *case_name,unsigned loop_count,int argc,char *argv[])
{
    int i,tc_count;
    argc=argc;
    argv=argv;

    tc_count = sizeof(g_rs422_test)/sizeof(struct utest_tc_export);
    if (!strcmp(case_name,"all")) {
        for (i=0;i<tc_count;i++) {
            utest_run(&g_rs422_test[i],loop_count);
        }
    }
    else {
        for (i=0;i<tc_count;i++) {
            if (!strcmp(case_name,g_rs422_test[i].name)) {
                break;
            }
        }

        if (i == tc_count) {
            display_usage();

            return -1;
        }

        utest_run(&g_rs422_test[i],loop_count);
    }

    return 0;
}
