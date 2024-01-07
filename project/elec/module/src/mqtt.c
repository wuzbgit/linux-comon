#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>
#include <pthread.h>
#include "ModParam.h"
#include "mqtt.h"

#define QOS         1
#define TIMEOUT     10000L

typedef struct {
    
}mqtt_sub_obj;

typedef struct {
    MQTTClient client;
    pthread_mutex_t mutex;
    bool bConnect;
    bool init;
}Vn_MqttClient_t;

static Vn_MqttClient_t vn_client = {0};

static int mqtt_connect(char *username,char *passwd,uint32_t timeout)
{
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = username; 
    conn_opts.password = passwd;
    conn_opts.connectTimeout = timeout;
    if ((rc = MQTTClient_connect(vn_client.client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("mqtt:Failed to connect, return code %d\n", rc);
        return -1;
    }    

    vn_client.bConnect = true;

    printf("mqtt connected\n");

    return 0;
}

static int mqtt_disconnect()
{
    MQTTClient_disconnect(vn_client.client, 10000);

    vn_client.bConnect = false;
    return 0;
}

int mqtt_is_connected()
{
    return vn_client.bConnect;
}

int mqtt_publish(char *topic,void *data,unsigned length)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    if (vn_client.bConnect == false) {
        return -1;
    }

    pthread_mutex_lock(&vn_client.mutex);
    pubmsg.payload = data;
    pubmsg.payloadlen = length;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;    

    MQTTClient_publishMessage(vn_client.client, topic, &pubmsg, &token);

    rc = MQTTClient_waitForCompletion(vn_client.client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);
    pthread_mutex_unlock(&vn_client.mutex);
    return rc;
}

int mqtt_subscribe(int topic,sub_callback fn)
{
    return 0;
}

int mqtt_unsubscribe(const char *topic)
{
    return 0;
}

int mqtt_start(char *uri,char *clientid,char *username,char *passwd,uint32_t timeout)
{
    int rc;

    rc = MQTTClient_create(&vn_client.client, uri, clientid,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != 0) {
        printf("mqtt:create client failed\n");

        return -1;
    }
    vn_client.init = true;
    rc = mqtt_connect(username,passwd,timeout);
    if (rc != 0) {
        MQTTClient_destroy(&vn_client.client);
        printf("mqtt:connect mqtt failed\n");
        return -1;
    }
    
    pthread_mutex_init(&vn_client.mutex, NULL);
    return 0;
}

int mqtt_stop(void)
{
    pthread_mutex_lock(&vn_client.mutex);
    mqtt_disconnect();

    MQTTClient_destroy(&vn_client.client);
    vn_client.init = false;
    pthread_mutex_unlock(&vn_client.mutex);
    return 0;
}
