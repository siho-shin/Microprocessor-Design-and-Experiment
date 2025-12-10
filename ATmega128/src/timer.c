#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "timer.h"

volatile long long ticks;

struct
{
	int arrival_time;
	int sleep_time;
	timerfunc_t handler;
}	waiting[WAITING_QUEUE_SZ];

ISR(TIMER1_COMPA_vect)
{
	ticks++;
}

void timer_init(void)
{
	int i;

	TCCR1A |= (1 << COM1A1);
	TCCR1B |= (1 << WGM12) | (1 << CS10);
	OCR1A = 16384;
	TIMSK |= (1 << OCIE1A);

	for (i = 0; i < WAITING_QUEUE_SZ; i++)
	{
		waiting[i].arrival_time = WAITING_QUEUE_INVALID;
		waiting[i].sleep_time = WAITING_QUEUE_INVALID;
		waiting[i].handler = 0;
	}

	sei();
}

void timer_sleep(int ms)
{
	int arrival_time;

	for (arrival_time = ticks; ticks < arrival_time + ms;);
}

int timer_notify(int ms, timerfunc_t func)
{
	return 0;
}
