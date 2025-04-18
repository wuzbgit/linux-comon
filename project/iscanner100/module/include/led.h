#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_GREEN,
    LED_RED,
    LED_YELLOW
}e_LED;

int led_on(e_LED led);

int led_off(e_LED led);

int led_blink(e_LED led);

#ifdef __cplusplus
}
#endif

#endif
