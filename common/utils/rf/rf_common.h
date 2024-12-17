#ifndef RF_COMMON_H
#define RF_COMMON_H

typedef enum {
    SIM_ABSENT = 0,
    SIM_NOT_READY = 1,
    SIM_READY = 2, /* SIM_READY means the radio state is RADIO_STATE_SIM_READY */
    SIM_PIN = 3,
    SIM_PUK = 4,
    SIM_NETWORK_PERSONALIZATION = 5,
    SIM_BAD = 6,
} SIM_Status;

typedef enum{
    RF_Qos_WORST = 0,
    RF_Qos_BAD,
    RF_Qos_NORMAL,
    RF_Qos_GOOD,
    RF_Qos_BEST,
}RF_Signal_Qos;

typedef enum{
	RF_NWMODE_NONE,
	RF_NWMODE_2G,
	RF_NWMODE_3G,
	RF_NWMODE_4G,
	RF_NWMODE_END
}RF_NWMODE;
#define DEF_RESERVE_NWMODE	RF_NWMODE_3G

typedef enum{
	RF_NONE = -1,
    RF_QUECTEL = 0,
    RF_LYNQ
}RF_MANUFACT;
	
typedef struct __RFSIGNAL {
    /*4G dynamic signal info*/
    char Freqband[16];
    char Opmode[32];
    char Sysmode[16];
    char Cpin[32];
    int RSRQ;
    int RSRP;
    int RSSI;
    int RSSNR;
    int SINR;
    int SignalQos;	/*5: Best; 4: Good; 3: Normal; 2: Bad; 1: Worst*/    
} RFSIGNAL_T;

typedef struct __RFINFO {
    /* 4G module info*/
    char Manufacture[16];
    char PLMN[16];
    char Model[16];
    char Rev[32];
    char SN[16];
    char IMEI[16];
    char CICCID[32];
    RFSIGNAL_T RFsignal;
	char SIMStatus[32];
	int rfnetConnectUtctime;
} RFINFO_T;

typedef struct __RFMODINFO {
    char Manufacture[16];
    char PLMN[16];
    char Model[16];
    char Rev[32];
    char SN[16];
}__RFMODINFO;

int poweron_rf(int gpio, int level);

int poweroff_rf(int gpio, int level);

int N1_RF_hwreset(int gpio, int on_level);

void print_RF_status(RFINFO_T* pstrfinfo_);

int find_RF_manufacture(const char* rfdev_);

int rf_at_test(char* rfdev_, const int rf_manufacture_);

int rf_get_nwmode(char* rfdev_, const int rf_manufacture_,RF_NWMODE *nwmode);

int rf_get_creg(char* rfdev_, const int rf_manufacture_,int *creg);

int rf_get_cpin(char* rfdev_, const int rf_manufacture_,int *cpin);

int rf_get_cfun(char* rfdev_, const int rf_manufacture_,int *cfun);

int rf_nw_scanmode(char* rfdev_, const int rf_manufacture_);

int rf_cfun_reset(char* rfdev_, const int rf_manufacture_);

int rf_soft_reset(char* rfdev_, const int rf_manufacture_);

int update_RF_signalInfo(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_);

int update_RF_status(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_);

int RF_device_detect(char * dev_, int size_);

int init_RF_info(RFINFO_T *RF_info);

int rf_at_cmd(char* rfdev_,char *cmd);

int get_RF_moduleInfo(const char* rfdev_, RFINFO_T* pstrfinfo_, int rf_mnf_);

int parse_RFinfo_request(const char* rfdev_, const char* atcmd_, char* resp_, unsigned int bufsize_);
#endif
