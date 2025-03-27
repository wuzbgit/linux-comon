#ifndef __MVCAM_H__
#define __MVCAM_H__

typedef enum {
    VIDEO_STREAM = 0,
    NOMAL_TRIGGER,
    ROLLING_SHTTER,
    PULSE_TRIGGER,
    UNKOWN_MODE
}MVCam_CapMode_e;

const char *mvcam_model_string(unsigned model);
const char *mvcam_model_string(unsigned model);

int mvcam_read_reg(unsigned reg, int *value);

int mvcam_write_reg(unsigned reg, int value);

int mvcam_read_manufacture(char *buf, unsigned buflen);

int mvcam_read_model(unsigned *model);

int mvcam_read_sensor_serial(unsigned *serial);

int mvcam_read_version(unsigned *version);

int mvcam_reboot_cam();

int mvcam_get_errcode(unsigned *errcode);

int mvcam_get_trigger_mode(MVCam_CapMode_e *mode);

int mvcam_set_trigger_mode(MVCam_CapMode_e mode);

int mvcam_get_trigger_source(int *hardware);

int mvcam_set_trigger_source(int hardware);

int mvcam_get_trigger_number(unsigned int *number);

int mvcam_set_trigger_number(unsigned number);

int mvcam_get_trigger_count(unsigned *count);

int mvcam_software_trigger();

int mvcam_get_trigger_delay(unsigned *ms);

int mvcam_set_trigger_delay(unsigned ms);

int mvcam_get_trigger_edge(int *falling_edge);

int mvcam_set_trigger_edge(int falling_edge);

int mvcam_get_gpio1_mode(int *strobe);

int mvcam_set_gpio1_mode(int strobe);

int mvcam_get_gpio1_status(int *level);

int mvcam_set_acquisition(int enable);

int mvcam_get_ae_mode(int *ae);

int mvcam_set_ae_mode(int ae);

int mvcam_get_ag_mode(int *ag);

int mvcam_set_ag_mode(int ag);

int mvcam_get_brightness(unsigned char *brightness);

int mvcam_set_brightness(unsigned char brightness);

#endif
