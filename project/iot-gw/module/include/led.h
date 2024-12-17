#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_USER0,
    LED_USER1,
    LED_USER2,
    LED_USER3,
    LED_USER4
}e_IOTGW_LED;

int led_on(e_IOTGW_LED led);

int led_off(e_IOTGW_LED led);

int led_blink(e_IOTGW_LED led);

#ifdef __cplusplus
}
#endif

#endif
