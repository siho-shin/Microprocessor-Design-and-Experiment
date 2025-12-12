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

void display(void)
{
	fnd_display_number_dot(ms, 2);
	timer_notify(2, display);
}

void mspp(void)
{
	if (on)
		ms++;
	timer_notify(100, mspp);
}

int main(void)
{
	init();
	
	display();
	mspp();

	schedule();

	while (1);
}
