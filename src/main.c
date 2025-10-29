#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "fnd.h"
#include "switch.h"
#include "led.h"

void left_to_right(void)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		led_set(LED(i));
		_delay_ms(50);
	}
}

void right_to_left(void)
{
	int i;

	for (i = 7; i >= 0; i--)
	{
		led_set(LED(i));
		_delay_ms(50);
	}
}

void init(void)
{
	fnd_init();
	switch_init(left_to_right, right_to_left);
	led_init();
}

int main(void)
{
	int i;

	init();
	for (i = 0; i < 10000; i++)
	{
		fnd_display_number(i);
		_delay_ms(50);
	}
}
