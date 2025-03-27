#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "mvcam.h"
#include "mvcam_register.h"

#define MVCAM_I2C_DEV   "/dev/i2c-0"
#define MVCAM_I2C_SLAVE 0x3b

struct MVCamSerial {
    unsigned serial;
    char *model_str;
};

static struct MVCamSerial cammodel[] = {
    {
        .serial = 0x178,
        .model_str = "MV-MIPI-IMX178M",
    },
    {
        .serial = 0x130,
        .model_str = "MV-MIPI-SC130M",
    },
    {
        .serial = 0x265,
        .model_str = "MV-MIPI-IMX265M",
    },
    {
        .serial = 0x264,
        .model_str = "MV-MIPI-IMX264M",
    },
    {
        .serial = 0x296,
        .model_str = "MV-MIPI-IMX296M",
    },
    {
        .serial = 0x287,
        .model_str = "MV-MIPI-IMX287M",
    },
    {
        .serial = 0x4002,
        .model_str = "MV-MIPIGMAX4002M",
    },
    {
        .serial = 0x8132,
        .model_str = "RAW-MIPI-SC132M",
    },
    {
        .serial = 0x8234,
        .model_str = "RAW-MIPIAR0234M",
    },
    {
        .serial = 0x8462,
        .model_str = "RAW-MIPIIMX462M",
    },
    {
        .serial = 0x8535,
        .model_str = "RAW-MIPI-SC535M",
    }              
};

static struct MVCamSerial sensor_serial[] = {
    {
        .serial = 0x178,
        .model_str = "IMX178LLJ",
    },
    {
        .serial = 0x130,
        .model_str = "SC130GS",
    },
    {
        .serial = 0x265,
        .model_str = "IMX265LLJ",
    },
    {
        .serial = 0x264,
        .model_str = "IMX264LLJ",
    },
    {
        .serial = 0x296,
        .model_str = "IMX296LLJ",
    },
    {
        .serial = 0x287,
        .model_str = "IMX287LLJ",
    },
    {
        .serial = 0x132,
        .model_str = "SC132GS",
    },
    {
        .serial = 0x234,
        .model_str = "AR0234CS",
    },
    {
        .serial = 0x462,
        .model_str = "RAW-IMX462LLR",
    },
    {
        .serial = 0x535,
        .model_str = "SC535HGS",
    },
    {
        .serial = 0x4002,
        .model_str = "GMAX4002",        
    }              
};

const char *mvcam_model_string(unsigned model)
{
    int i;
    for (i = 0;i < sizeof(cammodel); i++) {
        if (model == cammodel[i].serial) {
            return cammodel[i].model_str;
        }
    }

    return NULL;
}

const char *mvcam_sensor_string(unsigned sensor)
{
    int i;
    for (i = 0;i < sizeof(sensor_serial); i++) {
        if (sensor == sensor_serial[i].serial) {
            return sensor_serial[i].model_str;
        }
    }

    return NULL;
}

static int mvcam_i2c_read(uint16_t reg, uint32_t *values, uint32_t n)
{
	int err;
	int i = 0;
    int fd;
	uint8_t buf[2] = { reg >> 8, reg & 0xff };
    uint8_t bufout[8] = {0};
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2] = {
		{
			 .addr = MVCAM_I2C_SLAVE,
			 .flags = 0,
			 .len = 2,
			 .buf = buf,
		},
		{
			.addr = MVCAM_I2C_SLAVE,
			.flags = I2C_M_RD ,
			.len = 4,
			.buf = bufout,
		},
	};

	msgset.msgs = msgs;
	msgset.nmsgs = 2;

	fd = open(MVCAM_I2C_DEV, O_RDWR);
	if (!fd) {
		printf("Couldn't open I2C device %s\n",MVCAM_I2C_DEV);
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, MVCAM_I2C_SLAVE) < 0)
	{
		printf("Failed to set I2C address\n");
        close(fd);
		return -1;
	}

	err = ioctl(fd, I2C_RDWR, &msgset);
	if (err != msgset.nmsgs)
    {
        printf("Read i2c err %d\n", err);
        close(fd);
		return -1;
    }

	for(i = 0; i < n; ++i) {
        values[i] = ntohl(*(uint32_t*)bufout);
		fprintf(stderr,"addr %04x : value %08x \n",reg+i,values[i]);
	}

	close(fd);

	return 0;
}

static int mvcam_i2c_write(uint16_t reg, int data)
{
	int i;
	int fd;
	unsigned char msg[8] = {reg>>8, reg&0xff, data>>24, data>>16, data>>8,data,};
	int len = 6;
        
	fd = open(MVCAM_I2C_DEV, O_RDWR);
	if (!fd) {
		printf("Couldn't open I2C device\n");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, MVCAM_I2C_SLAVE) < 0) {
		printf("Failed to set I2C address\n");
        close(fd);
		return -1;
	}
	

    if (write(fd, msg, len) != len) {
		printf("Failed to write register index %d", i);
    }

    close(fd);

    return 0;
}

int mvcam_read_reg(unsigned reg, int *value)
{
    int ret;
    uint16_t reg_addr = reg;

    ret = mvcam_i2c_read(reg_addr, value, 1);
    // ret = i2c_rd(MVCAM_I2C_DEV, MVCAM_I2C_SLAVE, &reg_addr, 2, (unsigned char *)value, 4);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int mvcam_write_reg(unsigned reg, int value)
{
    int ret;
    uint16_t reg_addr = reg;
    ret = mvcam_i2c_write(reg_addr, value);
    // ret = i2c_wr(MVCAM_I2C_DEV, MVCAM_I2C_SLAVE, &reg_addr, 2, (unsigned char *)&value, 4);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int mvcam_read_manufacture(char *buf, unsigned buflen)
{
    int ret;
    uint16_t reg = Manufacturer_Name;
    if (buflen < 5) {
        return -1;
    }

    ret = i2c_rd(MVCAM_I2C_DEV, MVCAM_I2C_SLAVE, (unsigned char *)&reg, 2, buf, 4);
    if (ret <= 0) {
        return -1;
    }

    return 4;
}

int mvcam_read_model(unsigned *model)
{
    int ret;

    ret = mvcam_read_reg(Model_Name, (int *)model);
    
    return ret;
}

int mvcam_read_sensor_serial(unsigned *serial)
{
    int ret;

    ret = mvcam_read_reg(Sensor_Name, (int *)serial);
    
    return ret;
}

int mvcam_read_version(unsigned *version)
{
    int ret;

    ret = mvcam_read_reg(Device_Version, (int *)version);
    
    return ret;
}

int mvcam_reboot_cam()
{
    int ret;

    ret = mvcam_write_reg(System_reset, 1);

    return ret;
}

int mvcam_get_errcode(unsigned *errcode)
{
    int ret;

    ret = mvcam_read_reg(Error_code, (int *)errcode);
    
    return ret;
}

int mvcam_get_trigger_mode(MVCam_CapMode_e *mode)
{
    int ret;
    int value;
    
    ret = mvcam_read_reg(CAPTURE_MODE, (int *)&value);
    if (ret == 0) {
        if (value & 0x1) {
            *mode = VIDEO_STREAM;
        }
        else if (value & (1<<1)) {
            *mode = NOMAL_TRIGGER;
        }
        else if (value & (1<<1)) {
            *mode = ROLLING_SHTTER;
        }
        else if (value & (1<<1)) {
            *mode = PULSE_TRIGGER;
        }
        else {
            *mode = UNKOWN_MODE;
        }                
    }
    
    return ret;
}

int mvcam_set_trigger_mode(MVCam_CapMode_e mode)
{
    int ret = 0;
    int value;

    switch (mode)
    {
    case VIDEO_STREAM:
        value = 1 << 0;
        break;
    case NOMAL_TRIGGER:
        value = 1 << 1;
        break;
    case ROLLING_SHTTER:
        value = 1 << 2;
        break;
    case PULSE_TRIGGER:
        value = 1 << 3;
        break;
    default:
        ret = -1;
        break;
    } 

    if (ret != 0) {
        return -1;
    }

    ret = mvcam_write_reg(Trigger_Mode, 1);

    return ret;
}

int mvcam_set_trigger_source(int hardware)
{
    int ret;

    ret = mvcam_write_reg(Trigger_Source, hardware);

    return ret;
}

int mvcam_get_trigger_source(int *hardware)
{
    int ret;

    ret = mvcam_read_reg(CAPTURE_MODE, (int *)hardware);

    return ret;
}

int mvcam_set_trigger_number(unsigned number)
{
    int ret;

    ret = mvcam_write_reg(Trigger_Num, number);

    return ret;
}

int mvcam_get_trigger_number(unsigned int *number)
{
    int ret;

    ret = mvcam_read_reg(CAPTURE_MODE, (int *)number);

    return ret;
}

int mvcam_get_trigger_count(unsigned *count)
{
    int ret;

    ret = mvcam_read_reg(Trigger_Count, (int *)count);

    return ret;
}

int mvcam_software_trigger()
{
    int ret;

    ret = mvcam_write_reg(Trigger_Software, 1);

    return ret;
}

int mvcam_set_trigger_delay(unsigned ms)
{
    int ret;

    ret = mvcam_write_reg(Trigger_Delay, ms);

    return ret;
}

int mvcam_get_trigger_delay(unsigned *ms)
{
    int ret;

    ret = mvcam_read_reg(Trigger_Delay, (int *)&ms);

    return ret;
}

int mvcam_set_trigger_edge(int falling_edge)
{
    int ret;

    ret = mvcam_write_reg(Trigger_Activation, falling_edge);

    return ret;
}

int mvcam_get_trigger_edge(int *falling_edge)
{
    int ret;

    ret = mvcam_read_reg(Trigger_Activation, (int *)&falling_edge);

    return ret;
}

int mvcam_set_gpio1_mode(int strobe)
{
    int ret;
    int value = !strobe;

    ret = mvcam_write_reg(GPIO1_OutSelect, value);

    return ret;    
}

int mvcam_get_gpio1_mode(int *strobe)
{
    int ret;
    int value = 0;

    ret = mvcam_read_reg(GPIO1_OutSelect, (int *)&value);
    
    *strobe = !value;
    
    return ret;
}

int mvcam_get_gpio1_status(int *level)
{
    int ret;

    ret = mvcam_read_reg(GPIO1_OutStatus, (int *)level);
        
    return ret;    
}

int mvcam_set_acquisition(int enable)
{
    int ret;

    ret = mvcam_write_reg(Image_Acquisition, enable);

    return ret;
}

int mvcam_set_ae_mode(int ae)
{
    int ret;
    int mode = 0;

    if (ae) {
        mode = 2;
    }

    ret = mvcam_write_reg(Exposure_Mode, mode);

    return ret;
}

int mvcam_get_ae_mode(int *ae)
{
    int ret;
    int value = 0;

    ret = mvcam_read_reg(Exposure_Mode, (int *)&value);
    if (ret == 0) {
        if (value == 2) {
            *ae = 1;
        }
        else {
            *ae = 0;
        }
    }

    return ret;
}

int mvcam_set_ag_mode(int ag)
{
    int ret;
    int mode = 0;

    if (ag) {
        mode = 2;
    }

    ret = mvcam_write_reg(Gain_Mode, mode);

    return ret;
}

int mvcam_get_ag_mode(int *ag)
{
    int ret;
    int value = 0;

    ret = mvcam_read_reg(Gain_Mode, (int *)&value);
    if (ret == 0) {
        if (value == 2) {
            *ag = 1;
        }
        else {
            *ag = 0;
        }
    }

    return ret;
}

int mvcam_set_brightness(unsigned char brightness)
{
    int ret;

    ret = mvcam_write_reg(Target_Brightness, brightness);

    return ret;
}

int mvcam_get_brightness(unsigned char *brightness)
{
    int ret;
    int value = 0;

    ret = mvcam_read_reg(Trigger_Count, (int *)&value);
    
    *brightness = value;

    return ret;
}

