#ifndef __LED_H__
#define __LED_H__

#define LED(x)	(1 << (x))

void led_init(void);
void led_set(int leds);

#endif
