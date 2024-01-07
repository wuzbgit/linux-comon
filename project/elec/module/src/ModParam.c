#include <string.h>
#include <stdio.h>
#include "ModParam.h"

#define PARAM_ADDR  (0x0)
#define PARAM_MAGIC (0x5A)
#define PARAM_LEN   sizeof(ModParams_t)

#define HARDWARE_VERSION "v1.0.0"
#define SOFTWARE_VERSION "v1.0.0"

#define DEF_MQTT_PORT 1883
#define DEF_MQTT_URI      "tcp://mqtt-cn-7pp2mc57l01.mqtt.aliyuncs.com:1883"
#define DEF_MQTT_USERNAME "Signature|LTAI5tNT7wLMAMfLqoYxEcrB|mqtt-cn-7pp2mc57l01"
#define DEF_MQTT_CLIENTID "GID_PEP@@@usr0009"
#define DEF_MQTT_PASSWD   "k4AG/9A33tFBENof8bGb2BJ2HnQ="

#define DEF_NET_TYPE NET_TYPE_4G
// #define DEF_NET_TYPE NET_TYPE_WIFI

// #define WIFI_SSID     "2620"
// #define WIFI_PASSWORD "yzjd88888888"
// #define WIFI_SSID       "YP"
// #define WIFI_PASSWORD   "1q2w3e4r5t6y@"
#define WIFI_SSID       "XDS"
#define WIFI_PASSWORD   "62639209"
#define MQTT_PUB_PERIOD (60000)

typedef struct _ModParams
{
    /* data */
    uint8_t                   magic;
    uint8_t                   net_type;
    DeviceInfo_t                 dev_info;
    MqttSever_t                  mqtt_info;
    WifiAP_t                     wifi_info;
    SigalPhaseModuleAlarmEvent_t sigphase_info;
    ConPhaseModuleAlarmEvent_t   conphase_info;
    MqttTopic_t                  topic[MQTT_TOPIC_MAX];
} ModParams_t;

static ModParams_t  g_MetetParam;
// static const char * mqtt_topic[MQTT_TOPIC_MAX] = { "pep/dev",          "pep/display",
//                                                   "pep/event",        "pep/detail",
//                                                   "pep/setting/mqtt", "pep/setting/reset",
//                                                   "pep/setting/dev",  "pep/setting/topic",
//                                                   "pep/setting/param" };
static const char * mqtt_topic[MQTT_TOPIC_MAX] = { "pep/test",          "pep/test_display",
                                                  "pep/test_event",        "pep/test_detail",
                                                  "pep/setting/mqtt", "pep/setting/reset",
                                                  "pep/setting/dev",  "pep/setting/topic",
                                                  "pep/setting/param" };
static void ModParamReset(void)
{
    int i = 0;

    memset(&g_MetetParam, 0, PARAM_LEN);
    g_MetetParam.magic    = PARAM_MAGIC;
    g_MetetParam.net_type = DEF_NET_TYPE;

    strncpy(g_MetetParam.dev_info.label, "null", strlen("null") + 1);
    strncpy(g_MetetParam.dev_info.location, "null", strlen("null") + 1);
    strncpy(g_MetetParam.dev_info.type, "null", strlen("null") + 1);

    strncpy(g_MetetParam.mqtt_info.uri, DEF_MQTT_URI, strlen(DEF_MQTT_URI));
    g_MetetParam.mqtt_info.port = DEF_MQTT_PORT;
    // strncpy(g_MetetParam.mqtt_info.ip, DEF_MQTT_IP, strlen(DEF_MQTT_IP) + 1);
    strncpy(g_MetetParam.mqtt_info.clientid, DEF_MQTT_CLIENTID,
               strlen(DEF_MQTT_CLIENTID) + 1);
    strncpy(g_MetetParam.mqtt_info.username, DEF_MQTT_USERNAME,
               strlen(DEF_MQTT_USERNAME) + 1);
    strncpy(g_MetetParam.mqtt_info.password, DEF_MQTT_PASSWD, strlen(DEF_MQTT_PASSWD) + 1);

    strncpy(g_MetetParam.wifi_info.apname, WIFI_SSID, strlen(WIFI_SSID) + 1);
    strncpy(g_MetetParam.wifi_info.appasswd, WIFI_PASSWORD, strlen(WIFI_PASSWORD) + 1);
    for (i = 0; i < MQTT_TOPIC_MAX; i++)
    {
        g_MetetParam.topic[i].state  = MQTT_STA_ON;
        g_MetetParam.topic[i].period = MQTT_PUB_PERIOD;
    }
    g_MetetParam.topic[MQTT_PUB_DETAIL].state = MQTT_STA_OFF;
    g_MetetParam.topic[MQTT_PUB_EVT].period   = MQTT_PUB_PERIOD / 2;

    memset(&g_MetetParam.sigphase_info, 0, sizeof(SigalPhaseModuleAlarmEvent_t));
    memset(&g_MetetParam.conphase_info, 0, sizeof(ConPhaseModuleAlarmEvent_t));
    im3332_get_conphase_alarmevent(&g_MetetParam.conphase_info);
    im3332_get_sigalphase_alarmevent(&g_MetetParam.sigphase_info);
}

int8_t *ModParamGetTopicName(uint8_t id)
{
    if (id >= MQTT_TOPIC_MAX)
    {
        return NULL;
    }

    return (char *)mqtt_topic[id];
}

int32_t ModParamSetUri(int8_t *url, uint32_t port)
{
    if (!url || !port)
    {
        return -1;
    }

    g_MetetParam.mqtt_info.port = port;
    snprintf(g_MetetParam.mqtt_info.uri, MQTTSERVER_URL_MAXLENGTH, "tcp://%s:%u", url, port);

    return 0;
}

char *ModParamGetUri()
{
    return g_MetetParam.mqtt_info.uri;
}

int ModParamSetUserName(char *username)
{
    strncpy(g_MetetParam.mqtt_info.username,username,MQTTSERVER_USERNAME_MAXLENGTH-1);

    return 0;
}

char *ModParamGetUserName()
{
    return g_MetetParam.mqtt_info.username;
}

char *ModParamGetPasswd()
{
    return g_MetetParam.mqtt_info.password;
}

char *ModParamGetClientID()
{
    return g_MetetParam.mqtt_info.clientid;
}
int ModParamSetPasswd(char *passwd)
{
    strncpy(g_MetetParam.mqtt_info.password,passwd,MQTTSERVER_PASSWORD_MAXLENGTH-1);

    return 0;
}

uint32_t ModParamGetPort(void)
{
    return g_MetetParam.mqtt_info.port;
}
/*!
    \brief      convert hex 32bits value into unicode char
    \param[in]  value: Hex 32bits value
    \param[in]  pbuf: buffer pointer to store unicode char
    \param[in]  len: value length
    \param[out] none
    \retval     none
*/
static void int_to_str(uint32_t value, uint8_t *pbuf, uint8_t len)
{
    uint8_t index = 0U;

    for (index = 0U; index < len; index++)
    {
        if ((value >> 28U) < 0x0AU)
        {
            pbuf[index] = (uint8_t)((value >> 28) + '0');
        }
        else
        {
            pbuf[index] = (uint8_t)((value >> 28) + 'A' - 10U);
        }

        value = value << 4U;
    }

    pbuf[index] = 0U;
}
/* device unique ID */
#define DEVICE_ID1 (0x1FFFF7E8U) /* device ID1 */
#define DEVICE_ID2 (0x1FFFF7ECU) /* device ID2 */
#define DEVICE_ID3 (0x1FFFF7F0U) /* device ID3 */

int8_t *ModParamGetUUID(void)
{
    static char uuid[25] = " ";
    uint32_t DeviceSerial0, DeviceSerial1, DeviceSerial2;

    DeviceSerial0 = DEVICE_ID1;
    DeviceSerial1 = DEVICE_ID2;
    DeviceSerial2 = DEVICE_ID3;

    DeviceSerial0 += DeviceSerial2;
    // LOG_RAW("DeviceSerial0=0x%x,DeviceSerial1=0x%x,DeviceSerial2=0x%x\r\n",DeviceSerial0,DeviceSerial1,DeviceSerial2);
    int_to_str(DeviceSerial0, (uint8_t *)&(uuid[0]), 8U);
    int_to_str(DeviceSerial1, (uint8_t *)&(uuid[8]), 8U);
    int_to_str(DeviceSerial2, (uint8_t *)&(uuid[16]), 8U);

    uuid[24] = '\0';

    return uuid;
}

int32_t ModParamSetDeviceInfo(DeviceInfo_t dev_info)
{
    strncpy(g_MetetParam.dev_info.label, dev_info.label, strlen(dev_info.label) + 1);
    strncpy(g_MetetParam.dev_info.location, dev_info.location, strlen(dev_info.location) + 1);
    strncpy(g_MetetParam.dev_info.type, dev_info.type, strlen(dev_info.type) + 1);

    // TODO write the parameters to flash

    return 0;
}

DeviceInfo_t *ModParamGetDeviceInfo(void)
{
#if 0
    strncpy(dev_info->label, g_MetetParam.dev_info.label,
               strlen(g_MetetParam.dev_info.label) + 1);
    strncpy(dev_info->location, g_MetetParam.dev_info.location,
               strlen(g_MetetParam.dev_info.location) + 1);
    strncpy(dev_info->type, g_MetetParam.dev_info.type,
               strlen(g_MetetParam.dev_info.type) + 1);
     return 0;
#else
    return &g_MetetParam.dev_info;
#endif
}

int32_t ModParamConfigureNetwork(uint8_t net_type)
{
    g_MetetParam.net_type = net_type;

    // TODO write parameters to flash
    return 0;
}

uint8_t ModParamGetNetworkConfigure()
{
    return g_MetetParam.net_type;
}

int32_t ModParamConfigureWifi(WifiAP_t w_conf)
{
    strncpy(g_MetetParam.wifi_info.apname, w_conf.apname, strlen(w_conf.apname) + 1);
    strncpy(g_MetetParam.wifi_info.appasswd, w_conf.appasswd, strlen(w_conf.appasswd) + 1);
    return 0;
}

WifiAP_t *ModParamGetWifiInfo(void)
{
#if 0
    strncpy(w_conf->apname, g_MetetParam.wifi_info.apname,
               strlen(g_MetetParam.wifi_info.apname) + 1);
    strncpy(w_conf->appasswd, g_MetetParam.wifi_info.appasswd,
               strlen(g_MetetParam.wifi_info.appasswd) + 1);
    return 0;
#else
    return &g_MetetParam.wifi_info;
#endif
}

int32_t ModParamSetMqttServer(MqttSever_t *server)
{
    // strncpy(g_MetetParam.mqtt_info.uri, server.uri, strlen(server.uri) + 1);
    // strncpy(g_MetetParam.mqtt_info.ip, server.uri, strlen(server.ip) + 1);
    // g_MetetParam.mqtt_info.port = server.port;
    ModParamSetUri(server->uri, server->port);
    strncpy(g_MetetParam.mqtt_info.clientid, server->clientid, strlen(server->clientid) + 1);
    strncpy(g_MetetParam.mqtt_info.username, server->username, strlen(server->username) + 1);
    strncpy(g_MetetParam.mqtt_info.password, server->password, strlen(server->password) + 1);

    // TODO write paramters to Flash
    return 0;
}

MqttSever_t *ModParamGetMqttServer(void)
{
#if 0
    strncpy(server->uri, g_MetetParam.mqtt_info.uri,
               strlen(g_MetetParam.mqtt_info.uri) + 1);
    strncpy(server->uri, g_MetetParam.mqtt_info.ip, strlen(g_MetetParam.mqtt_info.ip) + 1);
    server->port = g_MetetParam.mqtt_info.port;
    strncpy(server->uri, g_MetetParam.mqtt_info.clientid,
               strlen(g_MetetParam.mqtt_info.clientid) + 1);
    strncpy(server->uri, g_MetetParam.mqtt_info.username,
               strlen(g_MetetParam.mqtt_info.username) + 1);
    strncpy(server->uri, g_MetetParam.mqtt_info.password,
               strlen(g_MetetParam.mqtt_info.password) + 1);

    return 0;
#else
    return &g_MetetParam.mqtt_info;
#endif
}

int8_t *ModParamGetHardwareVersion(void)
{
#if 0
    if (strlen(HARDWARE_VERSION) >= len)
    {
        /* code */
        return -1;
    }

    memcpy(version, HARDWARE_VERSION, strlen(HARDWARE_VERSION) + 1);

    return 0;
#else
    return HARDWARE_VERSION;
#endif
}

int8_t *ModParamGetSoftwareVersion(void)
{
#if 0
    if (strlen(SOFTWARE_VERSION) >= len)
    {
        /* code */
        return -1;
    }

    memcpy(version, SOFTWARE_VERSION, strlen(SOFTWARE_VERSION) + 1);

    return 0;
#else
    return SOFTWARE_VERSION;
#endif
}

int32_t ModParamSetTopicState(uint8_t id, unsigned period, unsigned state)
{
    if (id >= MQTT_TOPIC_MAX)
        return -1;

    g_MetetParam.topic[id].state  = state;
    g_MetetParam.topic[id].period = period;

    return 0;
}

int8_t ModParamGetState(uint8_t id)
{
    if (id >= MQTT_TOPIC_MAX)
        return -1;

    return g_MetetParam.topic[id].state;
}

uint32_t ModParamGetPeriod(uint8_t id)
{
    if (id >= MQTT_TOPIC_MAX)
        return 0;

    return g_MetetParam.topic[id].period;
}

int32_t ModParamSave(void)
{
    //TODO
    return 0;
}

int32_t param_set_default(void)
{
    ModParamReset();

    return 0;
}

SigalPhaseModuleAlarmEvent_t *ModParamsGetSigalPhaseEvent(void)
{
    return &g_MetetParam.sigphase_info;
}

int16_t ModParamsSetSigalPhaseEvent(SigalPhaseModuleAlarmEvent_t *info)
{
    int rc;

    rc = im3332_set_sigalphase_alarmevent(info);
    if (!rc) {
        memcpy(&g_MetetParam.sigphase_info, info, sizeof(g_MetetParam.sigphase_info));
    }

    return rc;
}

ConPhaseModuleAlarmEvent_t *ModParamsGetConPhaseEvent(void)
{
    return &g_MetetParam.conphase_info;
}

int16_t ModParamsSetConPhaseEvent(ConPhaseModuleAlarmEvent_t *info)
{
    int rc;
    rc = im3332_set_conphase_alarmevent(info);
    if (!rc) {
        memcpy(&g_MetetParam.conphase_info, info, sizeof(ConPhaseModuleAlarmEvent_t));
    }
    
    return rc;
}

int32_t ModParamInit(void)
{
    int32_t ret = 0;

    ModParamReset();

    return ret;
}