#include <avr/io.h>
#include <avr/iom128.h>

#include "led.h"

int current_led;

void led_init(void)
{
	DDRA |= 0xFF;
	current_led = 0;
}

void led_set(int leds)
{
	PORTA = leds;
}
