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
#define RS485_INST2_DEVPATH "/dev/ttyAS3"
#define RS485_INST2_ENABLE  "/sys/bus/platform/devices/2500c00.uart/rs485_enable"

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

#define RS485_INST1_PORT    42
#define RS485_INST2_PORT    109
int rs485_init()
{
    system("gpio -s PB10 1 0 0 0 > /dev/null");
    system("gpio -s PD13 1 0 0 0 > /dev/null");
    gpio_export(RS485_INST1_PORT);
    gpio_export(RS485_INST2_PORT);

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

void rs485_tx_enable(char *devpath)
{
    if (strcmp(RS485_INST1_DEVPATH,devpath) == 0) {
        system("gpio -s PB10 1 1 0 0 > /dev/null");
        system("gpio -s PB11 1 1 0 0 > /dev/null");
        usleep(5000);
    }
    else if(strcmp(RS485_INST2_DEVPATH,devpath) == 0) {
        system("gpio -s PD13 1 1 0 0 > /dev/null");
        usleep(5000);        
    }
}

void rs485_tx_disable(char *devpath)
{
    if (strcmp(RS485_INST1_DEVPATH,devpath) == 0) {
        system("gpio -s PB10 1 0 0 0 > /dev/null");
        system("gpio -s PB11 1 0 0 0 > /dev/null");
        usleep(5000);
    }
    else if(strcmp(RS485_INST2_DEVPATH,devpath) == 0) {
        system("gpio -s PD13 1 0 0 0 > /dev/null");
        usleep(5000);        
    }    
}

int rs485_echo_test(char *devpath)
{
    char rcvbuf[256] = {0};
    char sndbuf[256];
    int fd = 0;
    int n = 0;
    
    rs485_enable(devpath,1);

    fd = serial_open(devpath,10000);
    if (fd < 0) {
        printf("open %s failed\n",devpath);

        return -1;
    }
    
    n = sprintf(sndbuf,"rs485 %s echo test\n",devpath);
    write(fd, sndbuf, n);

    while(1) {

        n = read(fd, rcvbuf, sizeof(rcvbuf) - 1);
        if (n > 0) {  
            rcvbuf[n] = '\0';
            memset(sndbuf,0,sizeof(sndbuf));
            sprintf(sndbuf,"Received: %s\n",rcvbuf);
            printf("%s",sndbuf);
            n = strlen(sndbuf);

            write(fd, sndbuf, n);
            memset(rcvbuf,0,sizeof(rcvbuf));
        }
    }
 
    close(fd);

    return 0;
}

#include <time.h>
static long get_timestamp()
{
    time_t timestamp;

    timestamp = time(NULL);

    if (timestamp == -1) {
        perror("time");
        return 0;
    }

    return (long)timestamp;

    return 0;
}

int rs485_rw_test(char *devpath)
{
    char rcvbuf[256] = {0};
    char sndbuf[256];
    int fd = 0;
    int n = 0;
    long timestamp = 0;
    int ret = 0;

    rs485_enable(devpath,1);

    fd = serial_open(devpath,10000);
    if (fd < 0) {
        printf("open %s failed\n",devpath);

        return -1;
    }
    
    timestamp = get_timestamp();

    sprintf(sndbuf,"rs485 %s test:%ld\n",devpath,timestamp);
    write(fd, sndbuf, n);
    n = read(fd, rcvbuf, sizeof(rcvbuf) - 1);
    if (n > 0) {
        if (strcmp(rcvbuf,sndbuf) == 0) {
            ret = 0;
        }
    }

    if (ret == 0) {
        printf("rs485 %s test success\n",devpath);
    }
    else {
        printf("rs485 %s test fail\n",devpath);
    }

    return ret;
}

int rs422_echo_test(const char *devpath)
{
    char rcvbuf[256];
    char sndbuf[256];
    int fd = 0;
    int n = 0;
    
    rs485_enable(devpath,1);

    fd = serial_open(devpath,10000);
    if (fd < 0) {
        printf("open %s failed\n",devpath);

        return -1;
    }

    while(1) {
        n = read(fd, rcvbuf, sizeof(rcvbuf) - 1);
        if (n > 0) {  
            rcvbuf[n] = '\0';
            memset(sndbuf,0,sizeof(sndbuf));
            sprintf(sndbuf,"Received: %s\n",rcvbuf);
            printf("%s",sndbuf);
            n = strlen(sndbuf);
            write(fd, sndbuf, n);
        }
    }
      
    close(fd);

    return 0;
}

int rs485_loopback_test(char *dev_src,char *dev_dst)
{
    char rcvbuf[256] = {0};
    char sndbuf[256] = {0};
    int fd_src = 0,fd_dst = 0;
    int n = 0;
    long timestamp = 0;
    int state = -1;

    rs485_enable(dev_src,1);
    rs485_enable(dev_dst,1);

    fd_src = serial_open(dev_src,10000);
    if (fd_src <= 0) {
        printf("open %s failed\n",dev_src);

        return -1;
    }

    fd_dst = serial_open(dev_dst,10000);
    if (fd_dst <= 0) {
        printf("open %s failed\n",dev_dst);

        return -1;
    }

    timestamp = get_timestamp();
    n = sprintf(sndbuf,"rs485 %s test:%ld\n",dev_src,timestamp);
    write(fd_src, sndbuf, n);
    n = read(fd_dst, rcvbuf, sizeof(rcvbuf) - 1);
    if (n > 0) {
        if (strcmp(rcvbuf,sndbuf) == 0) {
            state = 0;
        }
    }
    else {
        printf("%s to %s read no data\n",dev_src,dev_dst);
    }

    close(fd_src);
    close(fd_dst);
    
    return state;

}
