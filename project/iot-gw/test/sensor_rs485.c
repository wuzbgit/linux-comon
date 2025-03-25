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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <sys/select.h>

#define BUF_SIZE 256

void print_usage(const char *prog_name) {
    printf("Usage: %s -d <device> -b <baudrate>\n", prog_name);
    printf("Options:\n");
    printf("  -d, --device     Specify serial device (e.g. /dev/ttyUSB0)\n");
    printf("  -b, --baudrate   Set baud rate (e.g. 9600, 115200)\n");
    printf("  -h, --help       Display this help message\n");
}

speed_t sensor_get_baudrate(unsigned int rate) {
    switch(rate) {
        case 1200:   return B1200;
        case 2400:   return B2400;
        case 4800:   return B4800;
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default:      return -1;
    }
}

int configure_serial(int fd, speed_t baudrate, unsigned timeout) {
    struct termios tty;
    if(tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        return -1;
    }

    cfsetispeed(&tty, baudrate);
    cfsetospeed(&tty, baudrate);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CLOCAL;
    tty.c_cflag |= CREAD;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = timeout;

    if(tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }
    return 0;
}

int sensor_test(char *devpath,unsigned baudrate)
{
    char rcvbuf[256] = {0};
    char sndbuf[256] = {0xFF,0x03,0x00,0x03,0x00,0x01,0x61,0xD4};
    int fd = 0;
    int n = 0, i;
    int ret = 0;

    fd = open(devpath, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        printf("invalid device:%s\n",devpath);

        exit(0);
    }
    rs485_enable(devpath,1);
    configure_serial(fd, sensor_get_baudrate(baudrate), 5);

    printf("write command to laser\n");

    while (1) {
        write(fd, sndbuf, 8);
        usleep(500000);
        // n = read(fd, rcvbuf, sizeof(rcvbuf) - 1);
        // printf("receive %d bytes\n",n);
        // if (n > 0) {
        //     for (i = 0;i < n; i++) {
        //         printf("0x%x ",rcvbuf[i]);
        //     }
        // }
        // else {
        //     printf("receive timeout\n");
        // }
    }

    return ret;
}

static struct option long_options[] = {
    {"device"        , required_argument, NULL, 'd'},
    {"baudrate"        , required_argument, NULL, 'b'},
    {"help"          , required_argument, NULL, 'h'}
};

int main(int argc,char *argv[])
{
    int opt;
    int option_idx = 0;
    char devpath[100] = {0};
    unsigned baudrate = 115200;

    if (argc == 1) {
        print_usage(argv[0]);

        return -1;
    }
    
    while((opt = getopt_long(argc,argv,"d:b:h?",long_options,&option_idx)) != -1) {
        switch (opt)
        {
        case 'd':
            strncpy(devpath,optarg,sizeof(devpath) - 1);
            break;
        case 'b':
            baudrate = atoi(optarg);
            break;
        case 'h':
        case '?':
            print_usage(argv[0]);
            break;
        default:
            break;
        }
    }

    if (strlen(devpath) == 0) {
        print_usage(argv[0]);

        return 0;
    }

    printf("device:%s baudrate:%d\n",devpath,baudrate);
    sensor_test(devpath,baudrate);

    return 0;
}
