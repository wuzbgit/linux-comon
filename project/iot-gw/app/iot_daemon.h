#ifndef _IOT_DAEMON_H_
#define _IOT_DAEMON_H_

#define SERVER_PROT             9091

#define CMD_RTC_INIT            0x2000
#define CMD_RTC_GET             0x2001
#define CMD_RTC_SET             0x2002
#define CMD_LED_SET           0x2003
#define CMD_DO_SET              0x2004
#define CMD_DI_GET              0x2005
#define CMD_AI_GET              0x2006
#define CMD_RS485_TEST          0x2007
#define CMD_RS422_TEST          0x2008
#define CMD_RF_RESET            0x2009
#define CMD_RF_MOD_GET          0x200a
#define CMD_RF_SIM_GET          0x200b
#define CMD_RF_SIM_DETECT_SET   0x200c

typedef struct rtc_info
{
    unsigned year;
    unsigned month;
    unsigned day;
    unsigned hour;
    unsigned min;
    unsigned sec;
}rtc_info_t;

#define OPT_LED_ON      0
#define OPT_LED_BLINK   1
#define OPT_LED_OFF     2

typedef struct led_info {
    unsigned opt;
    unsigned led_num;
}led_info_t;

typedef struct do_info {
    unsigned do_mask;
    unsigned level;
}do_info_t;

typedef struct di_info {
    unsigned di_mask;
    unsigned level;
}di_info_t;

typedef struct ai_info {
    unsigned ai_num;
    unsigned data;
}ai_info_t;

typedef struct rs485_test_info {
    unsigned instance;
    unsigned enable;
}rs485_test_info_t;

typedef struct rs422_test_info {
    unsigned instance;
    unsigned enable;
}rs422_test_info_t;

typedef struct rf_mod_info {
    /* 4G module info*/
    char Manufacture[16];
    char PLMN[16];
    char Model[16];
    char Rev[32];
    char SN[16];
    char IMEI[16];
    char CICCID[32];
	char SIMStatus[32];
} rf_mod_info_t;

typedef struct test_package {
    unsigned cmd;
    unsigned state;
    unsigned payloadlen;
    char payload[0];
}test_package_t;

#endif
