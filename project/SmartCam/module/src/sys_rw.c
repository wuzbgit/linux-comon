#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int read_sysfs_int(const char *path)
{
    char buf[64] = {0};
    ssize_t ret;
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return -1;
    }

    ret = read(fd, buf, sizeof(buf)-1);
    if (ret == -1) {
        perror("read failed");
        close(fd);
        return -1;
    }

    buf[ret] = '\0';
    close(fd);
    return (int)strtol(buf, NULL, 0);
}

int write_sysfs_int(const char *path, int value)
{
    char buf[64] = {0};
    int len;
    ssize_t ret;
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return -1;
    }

    len = snprintf(buf, sizeof(buf), "%d\n", value);

    ret = write(fd, buf, len);

    if (ret != len) {
        perror("write failed");
        close(fd);
        return -1;
    }
    return 0;
}

int read_sysfs_string(const char *path, char *buffer, unsigned buf_size)
{
    ssize_t ret;
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return -1;
    }

    ret = read(fd, buffer, buf_size-1);
    if (ret == -1) {
        perror("read failed");
        close(fd);
        return -1;
    }

    buffer[ret] = '\0';
    close(fd);

    return ret;
}

int write_sysfs_string(const char *path, const char *value)
{
    ssize_t ret;
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return -1;
    }

    ret = write(fd, value, strlen(value));
    close(fd);

    if (ret != (ssize_t)strlen(value)) {
        perror("write failed");
        return -1;
    }
    return 0;
}
