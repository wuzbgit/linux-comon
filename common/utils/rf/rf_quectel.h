#ifndef RF_QUECTEL_H
#define RF_QUECTEL_H
#include <stdbool.h>
#include "rf_common.h"
int update_RF_UESysInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_);

int get_RF_DeviceInfo_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_);

int get_RF_Signal_QUECTEL(const char* rfdev_, RFINFO_T* pstrfinfo_);

int soft_reset_QUECTEL(char* rfdev_);

int reset_cfun_QUECTEL(char* rfdev_);

int get_nwmode_QUECTEL(const char* rfdev_, RF_NWMODE *mode);

int get_cfun_QUECTEL(char* rfdev_,int *cfun);

int SimDetect_enable_QUECTEL(char* rfdev_,bool enable, int level);

int get_cpin_QUECTEL(char* rfdev_,int *cpinReady);

int get_creg_QUECTEL(char* rfdev_,int *creg);

int nwscanmode_QUECTEL(char* rfdev_);
#endif