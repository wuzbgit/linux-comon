#ifndef _ELE_PARAM_H_
#define _ELE_PARAM_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
#include "im3332.h"

#define NET_TYPE_4G   0x10
#define NET_TYPE_WIFI 0x11

#define MQTT_TOPICNAME_MAXLENGTH      30
#define MQTTSERVER_URL_MAXLENGTH      80
#define MQTT_CLIENTID_MAXLENGTH       30
#define MQTTSERVER_USERNAME_MAXLENGTH 60
#define MQTTSERVER_PASSWORD_MAXLENGTH 50
#define DEVICE_LABEL_MAX_LENGTH       30
#define DEVICE_LOCATION_MAX_LENGTH    50
#define DEVICE_TYPE_MAX_LENGTH        20

typedef struct
{
    /* data */
    int8_t label[DEVICE_LABEL_MAX_LENGTH];
    int8_t location[DEVICE_LOCATION_MAX_LENGTH];
    int8_t type[DEVICE_TYPE_MAX_LENGTH];
} DeviceInfo_t;

typedef struct
{
    /* data */
    int8_t apname[20];
    int8_t appasswd[20];
} WifiAP_t;

typedef struct
{
    /* data */
    int8_t uri[MQTTSERVER_URL_MAXLENGTH];
    unsigned port;
    int8_t     clientid[MQTT_CLIENTID_MAXLENGTH];
    int8_t     username[MQTTSERVER_USERNAME_MAXLENGTH];
    int8_t     password[MQTTSERVER_PASSWORD_MAXLENGTH];
} MqttSever_t;

typedef struct
{
    /* data */
    // int8_t topicname[MQTT_TOPICNAME_MAXLENGTH];
    uint8_t  state;
    uint32_t period; // unit:ms
} MqttTopic_t;

typedef enum mqtt_topic_type
{
    MQTT_PUB_DEV = 0,
    // MQTT_PUB_TOPIC,
    MQTT_PUB_DAT,
    MQTT_PUB_EVT,
    MQTT_PUB_DETAIL,
    MQTT_SUB_MQTT,
    MQTT_SUB_RST,
    MQTT_SUB_DEV,
    MQTT_SUB_TOPIC,
    MQTT_SUB_PARAM,
    MQTT_TOPIC_MAX,
} DeviceTopicType_t;

enum mqtt_topic_state
{
    MQTT_STA_OFF = 0,
    MQTT_STA_ON,
    MQTT_STA_MAX,
};

int8_t *ModParamGetTopicName(uint8_t id);
int32_t  ModParamSetUri(int8_t *url, uint32_t port);
char *ModParamGetUri();
int ModParamSetPasswd(char *passwd);
char *ModParamGetPasswd();
char *ModParamGetUserName();
int ModParamSetUserName(char *username);
uint32_t ModParamGetPort(void);
char *ModParamGetClientID();
/*
 *@fn int32_t ModParamInit();
 *
 *@brief Init device parameter module
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamInit(void);
/*
 *@fn int32_t ModParamGetUUID(unsigned *uuid);
 *
 *@brief get device uuid
 *@param etc
 *@return 0:success -1:fail
 * */
int8_t *ModParamGetUUID(void);
/*
 *@fn int32_t ModParamSetDeviceInfo(DeviceInfo_t dev_info);
 *
 *@brief setting device information
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamSetDeviceInfo(DeviceInfo_t dev_info);
/*
 *@fn int32_t ModParamGetDeviceInfo(DeviceInfo_t *dev_info);
 *
 *@brief get device information
 *@param etc
 *@return 0:success -1:fail
 * */
DeviceInfo_t *ModParamGetDeviceInfo(void);
/*
 *@fn int32_t ModParamConfigureNetwork();
 *
 *@brief configure network type
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamConfigureNetwork(uint8_t net_type);
/*
 *@fn int32_t ModParamConfigureNetwork();
 *
 *@brief get network configure type
 *@param etc
 *@return 0:success -1:fail
 * */
uint8_t ModParamGetNetworkConfigure(void);
/*
 *@fn int32_t ModParamConfigureWifi(WifiAP_t w_conf);
 *
 *@brief configure wifi ap
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamConfigureWifi(WifiAP_t w_conf);
/*
 *@fn int32_t ModParamConfigureWifi(WifiAP_t w_conf);
 *
 *@brief get wifi ap info
 *@param etc
 *@return 0:success -1:fail
 * */
// int32_t ModParamGetWifiInfo(WifiAP_t *w_conf);
WifiAP_t *ModParamGetWifiInfo(void);
/*
 *@fn int32_t ModParamSetMqttServer(MqttSever_t server);
 *
 *@brief update mqtt server
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamSetMqttServer(MqttSever_t *server);
/*
 *@fn int32_t ModParamSetMqttServer(MqttSever_t server);
 *
 *@brief get mqtt server info
 *@param etc
 *@return 0:success -1:fail
 * */
// int32_t ModParamGetMqttServer(MqttSever_t *server);
MqttSever_t *ModParamGetMqttServer();
/*
 *@fn int32_t ModParamGetHardwareVersion(int8_t *version,unsigned len);
 *
 *@brief get device hardware information
 *@param etc
 *@return 0:success -1:fail
 * */
// int32_t ModParamGetHardwareVersion(int8_t *version, unsigned len);
int8_t *ModParamGetHardwareVersion(void);
/*
 *@fn int32_t ModParamGetSoftwareVersion(int8_t *version,unsigned len);
 *
 *@brief get device software information
 *@param etc
 *@return 0:success -1:fail
 * */
// int32_t ModParamGetSoftwareVersion(int8_t *version, unsigned len);
int8_t *ModParamGetSoftwareVersion(void);
/*
 *@fn int32_t ModParamSetTopicState(DeviceTopicType_t type,unsigned state);
 *
 *@brief set mqtt topic state(enable or disable)
 *@param etc
 *@return 0:success -1:fail
 * */
int32_t ModParamSetTopicState(uint8_t id, unsigned period, unsigned state);
/*
 *@fn ModParamGetTopic(DeviceTopicType_t type,MqttTopic_t *topic);
 *
 *@brief get mqtt topic information
 *@param etc
 *@return 0:success -1:fail
 * */
// int32_t ModParamGetTopic(DeviceTopicType_t type, MqttTopic_t *topic);

int8_t   ModParamGetState(uint8_t id);
uint32_t ModParamGetPeriod(uint8_t id);

/*
 *@fn ModParamSave();
 *
 *@brief save device parameters to flash
 *@param none
 *@return 0:success -1:fail
 * */
int32_t ModParamSave(void);
/*
 *@fn ModParamSaveTopic();
 *
 *@brief save mqtt topic info to flash
 *@param none
 *@return 0:success -1:fail
 * */
// int32_t ModParamSaveTopic(DeviceTopicType_t type);

SigalPhaseModuleAlarmEvent_t *ModParamsGetSigalPhaseEvent(void);
ConPhaseModuleAlarmEvent_t *ModParamsGetConPhaseEvent(void);
#ifdef __cplusplus
}
#endif
#endif
