/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "ssi.h"

static const char *device = "/dev/spidev1.0";
static uint32_t mode;
static uint8_t bits = 17;
static uint32_t speed = 10000;

static int ssi_clk_configure(int fd_ssi,unsigned int speed)
{
	int ret = 0;
	/*
	 * max speed hz
	 */
	ret = ioctl(fd_ssi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");

		return ret;
	}

	ret = ioctl(fd_ssi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");

		return ret;
	}	

	return 0;
}
static int ssi_bytebits_configure(int fd_ssi,unsigned int bits)
{
	int ret = 0;
	/*
	 * max speed hz
	 */
	ret = ioctl(fd_ssi, SPI_IOC_WR_BITS_PER_WORD, &bits);	
	if (ret == -1)
	{
		perror("can't set bits per word");
		return ret;
	}

	ret = ioctl(fd_ssi, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
	{
		perror("can't set bits per word");

		return ret;
	}	

	return 0;
}
int ssi_ctrl(int fd_ssi,unsigned cmd,void *args)
{
	int ret = -1;
	unsigned int data;
	switch (cmd)
	{
	case SSI_CLK_CMD:
		data = *(unsigned int *)args;
		ret = ssi_clk_configure(fd_ssi, speed);
		break;
	case SSI_BYTEBITS_CMD:
		data = *(unsigned int *)args;
		ret = ssi_bytebits_configure(fd_ssi, data);
		break;
	default:
		break;
	}

	return ret;
}
int ssi_read(int fd_ssi, unsigned char *buf, int len, int timeout)
{
	int ret = 0;
	int out_fd;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)buf,
		.rx_buf = (unsigned long)buf,
		.len = len,
		.delay_usecs = timeout,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	if(fd_ssi < 0)
	{
		printf("device uninitialize,please init ssi firt\r\n");

		return 0;
	}

	ret = ioctl(fd_ssi, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
	{
		perror("can't send spi message");

		return 0;
	}
	
	return len;
}

int ssi_init(int clk)
{
	int ret;
	int fd_ssi;
	mode = SPI_CPOL | SPI_NO_CS;
	fd_ssi = open(device, O_RDWR);
	if(fd_ssi < 0) {
		printf("can not open ssi device\r\n");

		return -1;
	}

	/*
	 * spi mode
	 */
	ret = ioctl(fd_ssi, SPI_IOC_WR_MODE32, &mode);
	if (ret < 0)
	{
		printf("can't set spi mode\r\n");

		goto ssi_init_err;
	}
		
	ret = ioctl(fd_ssi, SPI_IOC_RD_MODE32, &mode);
	if (ret < 0)
	{
		printf("can't get spi mode\r\n");

		goto ssi_init_err;
	}

	ret = ssi_bytebits_configure(fd_ssi, bits);
	if(ret != 0) {
		goto ssi_init_err;
	}

	if(clk > 0)
		speed = clk;
		
	ret = ssi_clk_configure(fd_ssi, speed);
	if(ret != 0)
		goto ssi_init_err;

	return fd_ssi;
ssi_init_err:
	close(fd_ssi);
	fd_ssi = -1;	
	return -1;	
}

int ssi_deinit(int fd_ssi)
{
	if(fd_ssi > 0)
	{
		close(fd_ssi);
		fd_ssi = -1;
	}
}