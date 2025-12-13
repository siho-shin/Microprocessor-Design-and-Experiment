#include <avr/io.h>
#include <avr/iom128.h>

#include "led.h"

volatile char led_is_init;

void led_init(void)
{
	if (led_is_init)
		return;

	DDRA |= 0xFF;
	led_is_init = 1;
}

void led_set(int leds)
{
	PORTA = leds;
}
