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
#include "sd.h"
#include "task.h"

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

void display(void)
{
	fnd_display_number_dot(ms, 2);
}

void mspp(void)
{
	if (on)
		ms++;
}

volatile char is_playing;

void upper_switch(void)
{
	if (is_playing)
	{
		// unimplemented
	}
	else
	{
		
	}
}

void lower_switch(void)
{

}

void init_failed(void)
{
	while (1) fnd_display_number(4444);
}

void init_succeed(void)
{
	
}

void init(void)
{
	buzzer_init();
	fnd_init();
	led_init();
	switch_init(start_timer, end_timer);
	//switch_init(upper_switch, lower_switch);
	timer_init();

	/*
	if (!sd_init())
		init_failed();
		*/
}

TASK(display, 2, TASK_NO_CONDITION);
TASK(mspp, MS(100), TASK_NO_CONDITION);

int main(void)
{
	init();
	
//	display();
//	mspp();

	START_TASK(display);
	START_TASK(mspp);

	schedule();
	while (1);
}
