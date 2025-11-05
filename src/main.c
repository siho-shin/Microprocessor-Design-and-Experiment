#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "buzzer.h"
#include "fnd.h"
#include "led.h"
#include "switch.h"

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
	buzzer_init();
	fnd_init();
	led_init();
	switch_init(left_to_right, right_to_left);
}

int main(void)
{
	init();

	while (1)
		fnd_display_number(7);
}
