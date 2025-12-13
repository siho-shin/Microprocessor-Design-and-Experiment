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
#include "timer.h"

volatile int on;
volatile int ms;

extern volatile long long ticks; // DEBUG

void start_timer(void)
{
	on = 1 - on;
}

void end_timer(void)
{
	ms = 0;
}

void init(void)
{
	buzzer_init();
	fnd_init();
	led_init();
	switch_init(start_timer, end_timer);
	timer_init();
}

void display_0(void)
{
	fnd_display_one_number((ms / 1000) % 10, 0);
	timer_notify(2, display_0);
}

void display_1(void)
{
	fnd_display_one_number((ms / 100) % 10, 1);
	timer_notify(2, display_1);
}

void display_2(void)
{
	fnd_display_one_number((ms / 10) % 10, 2);
	timer_notify(2, display_2);
}

void display_3(void)
{
	fnd_display_one_number((ms / 1) % 10, 3);
	timer_notify(2, display_3);
}

void display(void)
{
	// fnd_display_number_dot(ms, 2);
	timer_notify(100, display);
}

void mspp(void)
{
	if (on)
		ms++;
	timer_notify((long long)100, mspp);
}

int main(void)
{
	init();
	
//	display();
	display_0();
	display_1();
	display_2();
	display_3();

	mspp();
	schedule();
	while (1);
}
