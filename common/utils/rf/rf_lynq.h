#ifndef RF_LYNQ_H
#define RF_LYNQ_H
#include "rf_common.h"

int get_RF_DeviceInfo_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_);

int get_RF_Signal_LYNQ(const char* rfdev_, RFINFO_T* pstrfinfo_);

int soft_reset_LYNQ(char* rfdev_);

int reset_cfun_LYNQ(char* rfdev_);

int get_cfun_LYNQ(char* rfdev_,int *cfun);

int get_cpin_LYNQ(char* rfdev_,int *cpinReady);

int get_creg_LYNQ(char* rfdev_,int *creg);
#endif