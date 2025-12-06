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

volatile int on;
volatile int ms;
volatile int acc;

void start_timer(void)
{
	on = 1 - on;
}

void end_timer(void)
{
	ms = 0;
	acc = 0;
}

ISR(TIMER1_COMPA_vect)
{
	if (on)
	{
		acc++;
		if (acc >= 100)
		{
			acc = 0;
			ms++;
		}
	}
}

void init(void)
{
	buzzer_init();
	fnd_init();
	led_init();
	switch_init(start_timer, end_timer);

	TCCR1A |= (1 << COM1A1);
	TCCR1B |= (1 << WGM12) | (1 << CS10);
	OCR1A = 16384;

	TIMSK |= (1 << OCIE1A);

	ms = 0;
	acc = 0;
	sei();
}

int main(void)
{
	init();
	
	while (1)
		fnd_display_number_dot(ms, 2);
}
