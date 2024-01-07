#ifndef MQTT_H_
#define MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*sub_callback)(int topic,void *data,unsigned length);

int mqtt_is_connected();

int mqtt_publish(char *topic,void *data,unsigned length);

int mqtt_subscribe(int topic,sub_callback fn);

int mqtt_unsubscribe(const char *topic);

int mqtt_start(char *uri,char *clientid,char *username,char *passwd,uint32_t timeout);

int mqtt_stop(void);

#ifdef __cplusplus
}
#endif
#endif
