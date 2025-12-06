#define F_CPU	16000000UL
#include <avr/io.h>
#include <avr/iom128.h>
#include <util/delay.h>
#include "buzzer.h"

void dynamic_delay_us(int us)
{
	int i;

	for (i = 0; i < us; i++)
		_delay_us(1);
}

void buzzer_init(void)
{
	DDRB |= (1 << PB4);
}

void buzzer(int hz)
{
	int us;
	
	if (hz == 0)
		return;
	us = (int)(500000 / (float)hz);

	PORTB |= (1 << PB4);
	dynamic_delay_us(us);
	PORTB &= ~(1 << PB4);
	dynamic_delay_us(us);
}
