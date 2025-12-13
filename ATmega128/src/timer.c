#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "timer.h"

// debug
#include <util/delay.h>
#include "led.h"
#include "fnd.h"
#include "debug.h"

volatile long long ticks;

struct wq
{
	long long arrival_time;
	long long sleep_time;
	timerfunc_t handler;
}	waiting[WAITING_QUEUE_SZ];

// front          back_excl
// [0]  [1]  [2]  [x]  ...
int front = 0;
int back_excl = 1;
int has_init = 0;

void reset_waiting_queue_element(int i)
{
	waiting[i].arrival_time = WAITING_QUEUE_INVALID;
	waiting[i].sleep_time = 0;
	waiting[i].handler = 0;
}

void circular_increment(int *ptr)
{
	*ptr = (*ptr + 1) % WAITING_QUEUE_SZ;
}

int is_passed(int index, long long cur_ticks)
{
	return waiting[index].arrival_time + waiting[index].sleep_time <= cur_ticks;
}

// empty:
//                front
//                back_excl
// [x]  [x]  [x]  [x]  [x]  [x]  ...
//
// full :
//                front
//                back_excl
// [0]  [1]  [2]  [3]  [4]  [5]  ...
int is_queue_empty(void)
{
	return front == back_excl && waiting[front].arrival_time == WAITING_QUEUE_INVALID;
}

int is_queue_full(void)
{
	return front == back_excl && waiting[front].arrival_time != WAITING_QUEUE_INVALID;
}

int is_insertable(timerfunc_t handler)
{
	int i;

	for (i = front; i != back_excl; circular_increment(&i))
		if (waiting[i].handler == handler)
			return 0;
	return 1;
}

void swap(int i, int j)
{
	struct wq temp;

	temp = waiting[i];
	waiting[i] = waiting[j];
	waiting[j] = temp;
}

void sort(void)
{
	int i, j;
	int min;

	//debug(MOD_TIMER, SORT);
	for (i = front; i != back_excl; circular_increment(&i))
	{
		min = i;

		debug(MOD_TIMER, SORT1);
		for (j = i; j != back_excl; circular_increment(&j))
		{
			if (waiting[j].arrival_time == WAITING_QUEUE_INVALID)
			{
				while (1)
				{
					fnd_display_number(front * 100 + back_excl);
					led_set(0b11100111);
				}
			}

			if (waiting[j].arrival_time + waiting[j].sleep_time < waiting[min].arrival_time + waiting[min].sleep_time)
				min = j;
		}

		swap(min, i);
		debug(MOD_TIMER, SORT2);
	}
	debug(MOD_TIMER, SORT3);
}

int insert_to_queue(long long sleep_time, timerfunc_t handler)
{
	long long cur_ticks = ticks;
	int pos = has_init ? back_excl : 0;

	//debug(MOD_TIMER, INSERT_TO_QUEUE);
	while (is_queue_full());// || !is_insertable(handler));

	cli();
	waiting[pos].arrival_time = cur_ticks;
	waiting[pos].sleep_time = sleep_time;
	waiting[pos].handler = handler;
	if (has_init)
		circular_increment(&back_excl);
	else
		has_init = 1;
	sort();
	sei();

	return 0;
}

void set_off(long long set_off_time)
{
	//debug(MOD_TIMER, SET_OFF);

	while (!is_queue_empty() && is_passed(front, set_off_time))
	{
		waiting[front].handler();
		cli();
		reset_waiting_queue_element(front);
		circular_increment(&front);
		sei();
	}
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
	OCR1A = 4096;
	TIMSK |= (1 << OCIE1A);

	for (i = 0; i < WAITING_QUEUE_SZ; i++)
		reset_waiting_queue_element(i);
	sei();
}

void timer_notify(long long ms, timerfunc_t func)
{
	insert_to_queue(ms, func);
}

void schedule(void)
{
	long long cur_ticks;

	while (1)
	{
		cur_ticks = ticks;

		if (is_queue_empty())
		{
			led_set(0b11000011);
			continue;
		}
		if (is_passed(front, cur_ticks))
			set_off(cur_ticks);
	}
}
