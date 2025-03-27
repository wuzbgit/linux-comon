#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_USER0,
    LED_USER1,
    LED_USER2
}LED_e;

int led_on(LED_e led);

int led_off(LED_e led);

int led_blink(LED_e led);

#ifdef __cplusplus
}
#endif

#endif
