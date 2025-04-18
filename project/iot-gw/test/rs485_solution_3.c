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
#include "gpio/gpio.h"

#define PORT_BAUDRATE       B115200
// #define PORT_BAUDRATE       B57600
// #define PORT_BAUDRATE       B19200
// #define PORT_BAUDRATE       B9600
// #define PORT_BAUDRATE       B4800
// #define PORT_BAUDRATE       B2400
// #define PORT_BAUDRATE       B1800
// #define PORT_BAUDRATE       B1200
#define RS485_INST1_DEVPATH "/dev/ttyAS1"
#define RS485_INST1_ENABLE  "/sys/bus/platform/devices/2500400.uart/rs485_enable"
#define RS485_INST1_TX_DONE "/sys/bus/platform/devices/2500400.uart/rs485_tx_done"
#define RS485_INST1_GPIO_PORT     "PB10"
#define RS485_INST2_DEVPATH "/dev/ttyAS3"
#define RS485_INST2_ENABLE  "/sys/bus/platform/devices/2500c00.uart/rs485_enable"
#define RS485_INST2_TX_DONE "/sys/bus/platform/devices/2500c00.uart/rs485_tx_done"
#define RS485_INST2_GPIO_PORT     "PD13"

int rs485_gpio_init(char *port)
{
    char cmd[64] = {0};
    int ret;
    sprintf(cmd,"gpio -s %s 1 0 0 0 > /dev/null");
    ret = system(cmd);

    return ret;
}

int rs485_tx_enable(char *port)
{
    char cmd[64] = {0};
    int ret;
    sprintf(cmd,"gpio -s %s 1 1 0 0 > /dev/null");
    ret = system(cmd);

    return ret;    
}

int rs485_tx_disable(char *port)
{
    char cmd[64] = {0};
    int ret;
    sprintf(cmd,"gpio -s %s 1 0 0 0 > /dev/null");
    ret = system(cmd);

    return ret;    
}

int rs485_enable(const char *devpath,int enable)
{
    char cmd[64] = {0};
    int ret = -1;

    if (strcmp(devpath,RS485_INST1_DEVPATH) == 0) {
        sprintf(cmd,"echo %d > %s",enable,RS485_INST1_ENABLE);
    }
    else if (strcmp(devpath,RS485_INST2_DEVPATH) == 0) {
        sprintf(cmd,"echo %d > %s",enable,RS485_INST2_ENABLE);
    }

    if (strlen(cmd) != 0) {
        ret = system(cmd);
    }

    return ret;
}

ssize_t rs485_tx_done(const char *node_path)
{
    int fd;
    ssize_t bytes_read;
    char buffer[10] = {0};

    fd = open(node_path, O_RDONLY);
    if (fd == -1) {
        perror("can not open file");
        return -errno;
    }
 
    bytes_read = read(fd, buffer, 10);
    if (bytes_read == -1) {
        perror("read failed");
        close(fd);
        return -errno;
    }
 
    close(fd);
 
    if(buffer[0] == 1)
        return 1;
    else 
        return 0;
}

int serial_open(const char *devpath,unsigned timeout)
{
    int serial_port = open(devpath, O_RDWR);  

    if (serial_port < 0) {  
        perror("Error opening serial port");  
        return -1;  
    }

    struct termios tty;  
    memset(&tty, 0, sizeof tty);  
  
    if (tcgetattr(serial_port, &tty) != 0) {  
        perror("Error from tcgetattr");  
        return -1;  
    }  
  
    cfsetospeed(&tty, PORT_BAUDRATE);  
    cfsetispeed(&tty, PORT_BAUDRATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  
    tty.c_cflag |= (CLOCAL | CREAD);  
    tty.c_cflag &= ~(PARENB | PARODD);  
    tty.c_cflag &= ~CSTOPB;  
    tty.c_cflag &= ~CRTSCTS;  
  
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  
    tty.c_lflag = 0;  
    tty.c_oflag = 0;  
  
    tty.c_cc[VMIN] = 0;  
    tty.c_cc[VTIME] = timeout;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {  
        perror("Error from tcsetattr");  
        return -1;  
    }

    return serial_port;
}

int rs485_echo_test(char *devpath)
{
    char rcvbuf[256] = {0};
    char sndbuf[256];
    int fd = 0;
    int n = 0;
    
    fd = serial_open(devpath,10000);
    if (fd < 0) {
        printf("open %s failed\n",devpath);

        return -1;
    }
    
    n = sprintf(sndbuf,"rs485 %s echo test\n",devpath);
    rs485_tx_enable(RS485_INST1_GPIO_PORT);
    write(fd, sndbuf, n);
    while(rs485_tx_done == 0) {
        usleep(10*1000);
    }
    rs485_tx_disable(RS485_INST1_GPIO_PORT);
    while(1) {

        n = read(fd, rcvbuf, sizeof(rcvbuf) - 1);
        if (n > 0) {  
            rcvbuf[n] = '\0';
            memset(sndbuf,0,sizeof(sndbuf));
            sprintf(sndbuf,"Received: %s\n",rcvbuf);
            printf("%s",sndbuf);
            n = strlen(sndbuf);

            rs485_tx_enable(RS485_INST1_GPIO_PORT);
            write(fd, sndbuf, n);
            while(rs485_tx_done == 0) {
                usleep(10*1000);
            }
            rs485_tx_disable(RS485_INST1_GPIO_PORT);
            memset(rcvbuf,0,sizeof(rcvbuf));
        }
    }
 
    close(fd);

    return 0;
}

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

    int ret;
    rs485_gpio_init(RS485_INST1_GPIO_PORT);
    ret = rs485_enable("/dev/ttyAS1",0);
    if (ret != 0) {
        printf("invalid parameters\n");

        return 0;
    }

    rs485_echo_test("/dev/ttyAS1");

    return 0;
}