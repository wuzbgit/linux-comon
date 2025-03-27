#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

int i2c_rd(const char *i2c_dev, unsigned char slave_addr, unsigned char *reg, unsigned reglen, unsigned char *buf, unsigned buflen)
{
	int err;
	int i = 0;
    int fd;
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2] = {
		{
			 .addr = slave_addr,
			 .flags = 0,
			 .len = reglen,
			 .buf = reg,
		},
		{
			.addr = slave_addr,
			.flags = I2C_M_RD ,
			.len = buflen,
			.buf = buf,
		},
	};

	msgset.msgs = msgs;
	msgset.nmsgs = 2;
    fd = open(i2c_dev, O_RDWR);
    if (fd <= 0) {
		printf("Couldn't open I2C device\n");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr) < 0) {
		printf("Failed to set I2C address\n");
        close(fd);
		return -1;
	}

	err = ioctl(fd, I2C_RDWR, &msgset);
	if (err != msgset.nmsgs) {
        printf("Read i2c err %d\n", err);
        close(fd);
		return -1;
    }

	close(fd);

	return buflen;
}

int i2c_wr(const char *i2c_dev, unsigned char slave_addr, unsigned char *reg, unsigned reg_len, unsigned char *buf, unsigned buflen)
{
	int err;
	int i = 0;
    int fd;
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2] = {
		{
			 .addr = slave_addr,
			 .flags = 0,
			 .len = reg_len,
			 .buf = reg,
		},
		{
			.addr = slave_addr,
			.flags = 0,
			.len = buflen,
			.buf = buf,
		},
	};

	msgset.msgs = msgs;
	msgset.nmsgs = 2;
    fd = open(i2c_dev, O_RDWR);
    if (fd <= 0) {
		printf("Couldn't open I2C device\n");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr) < 0) {
		printf("Failed to set I2C address\n");
        close(fd);
		return -1;
	}

	err = ioctl(fd, I2C_RDWR, &msgset);
	if (err != msgset.nmsgs) {
        printf("Read i2c err %d\n", err);
        close(fd);
		return -1;
    }

	close(fd);

    return buflen;
}
