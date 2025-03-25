#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <sys/select.h>

#define BUF_SIZE 256

static struct option long_options[] = {
    {"device",    required_argument, 0, 'd'},
    {"baudrate",  required_argument, 0, 'b'},
    {"help",      no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

void print_usage(const char *prog_name) {
    printf("Usage: %s -d <device> -b <baudrate>\n", prog_name);
    printf("Options:\n");
    printf("  -d, --device     Specify serial device (e.g. /dev/ttyUSB0)\n");
    printf("  -b, --baudrate   Set baud rate (e.g. 9600, 115200)\n");
    printf("  -h, --help       Display this help message\n");
}

speed_t get_baudrate(unsigned int rate) {
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

int main(int argc, char *argv[]) {
    char *device = NULL;
    int baudrate = 0;
    int opt, fd;

    if (argc == 1) {
        print_usage(argv[0]);
        exit(0);
    }

    while ((opt = getopt_long(argc, argv, "d:b:h?", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                device = optarg;
                break;
            case 'b':
                baudrate = atoi(optarg);
                break;
            case 'h':
            case '?':
                print_usage(argv[0]);
                break;
        }
    }

    if (!device || (fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
        exit(EXIT_FAILURE);

    if (configure_serial(fd, get_baudrate(baudrate), 1000000) < 0)
        exit(EXIT_FAILURE);

    fd_set fds;
    char buf[BUF_SIZE];
    write(fd, "rs485 echo test\n", strlen("rs485 echo test\n"));
    while (1) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        FD_SET(STDIN_FILENO, &fds);
        memset(buf,sizeof(buf),0);
        if (select(fd + 1, &fds, NULL, NULL, NULL) > 0) {

            if (FD_ISSET(fd, &fds)) {
                int n = read(fd, buf, BUF_SIZE);
                if (n > 0) {
                    printf("receive %d bytes:%s\n",n,buf);
                    write(fd, buf, n);
                }
            }
        }
    }

    close(fd);
    return 0;
}

