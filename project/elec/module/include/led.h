#ifndef _LED_H_
#define _LED_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_4G,
    LED_IM3332,
    LED_SYSTEM
}e_ELEC_LED;

int led_on(e_ELEC_LED led);

int led_off(e_ELEC_LED led);

int led_blink(e_ELEC_LED led);

#ifdef __cplusplus
}
#endif

#endif
