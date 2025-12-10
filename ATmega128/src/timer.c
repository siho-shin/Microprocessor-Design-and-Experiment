#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "timer.h"

// debug
#include "led.h"
#include "fnd.h"

volatile long long ticks;

struct wq
{
	long long arrival_time;
	int sleep_time;
	timerfunc_t handler;
}	waiting[WAITING_QUEUE_SZ];

int waiting_cnt;

void reset_waiting_queue_element(int i)
{
	waiting[i].arrival_time = WAITING_QUEUE_INVALID;
	waiting[i].sleep_time = 0;
	waiting[i].handler = 0;
}

void swap(int i, int j)
{
	struct wq temp;

	temp = waiting[i];
	waiting[i] = waiting[j];
	waiting[j] = temp;
}

void push_back(int from_index)
{
	int i;

	for (i = WAITING_QUEUE_SZ - 1; i > from_index; i--)
		swap(i, i - 1);
}

void push_front(int from_index)
{
	int i;

	for (i = 0; i < WAITING_QUEUE_SZ; i++)
		swap(i, i + 1);
}

int insert_to_queue(int sleep_time, timerfunc_t handler)
{
	int i;
	long long cur_ticks = ticks;

	for (i = 0; i < WAITING_QUEUE_SZ; i++)
		if (cur_ticks + sleep_time <= waiting[i].arrival_time + waiting[i].sleep_time)
			break;
	push_back(i);

	waiting[i].arrival_time = cur_ticks;
	waiting[i].sleep_time = sleep_time;
	waiting[i].handler = handler;
	waiting_cnt++;

	return 0;
}

void set_off(long long set_off_time)
{
	int i, j;

	cli();
	for (i = 0; set_off_time >= waiting[0].arrival_time + waiting[0].sleep_time; i++)
	{
		waiting[0].handler();
		push_front(0);
	}
	waiting_cnt -= i;
	for (j = WAITING_QUEUE_SZ - 1; i >= 0; i--, j--)
		reset_waiting_queue_element(j);
	sei();
}

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
		reset_waiting_queue_element(i);

	sei();
}

void timer_sleep(int ms)
{
	int arrival_time;

	for (arrival_time = ticks; ticks < arrival_time + ms;);
}

int timer_notify(int ms, timerfunc_t func)
{
	cli();
	if (waiting_cnt >= WAITING_QUEUE_SZ)
		goto fail;
	if (insert_to_queue(ms, func))
		goto fail;
	sei();
	return 0;

fail:
	sei();
	return -1;
}

void schedule(void)
{
	long long cur_ticks;

	while (1)
	{
		cur_ticks = ticks;

		fnd_display_number((waiting[0].arrival_time + waiting[0].sleep_time) % 10000);

		if (waiting[0].arrival_time == WAITING_QUEUE_INVALID)
			continue;
		if (waiting[0].arrival_time + waiting[0].sleep_time <= cur_ticks)
			set_off(cur_ticks);
	}
}
