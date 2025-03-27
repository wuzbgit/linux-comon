#ifndef __I2C_RW_H__
#define __I2C_RW_H__

int i2c_rd(const char *i2c_dev, unsigned char slave_addr, unsigned char *reg, unsigned reglen, unsigned char *buf, unsigned buflen);

int i2c_wr(const char *i2c_dev, unsigned char slave_addr, unsigned char *reg, unsigned reg_len, unsigned char *buf, unsigned buflen);

#endif
