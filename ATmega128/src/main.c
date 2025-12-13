#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <util/delay.h>
#include <stdint.h>

#include "buzzer.h"
#include "fnd.h"
#include "led.h"
#include "switch.h"
#include "timer.h"
#include "sd.h"
#include "task.h"

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

/**************************************************** INIT ****************************************************/ 
int booting = 0;

void booting_screen(void)
{
	fnd_display_number(
		booting * 1000
		+ booting * 100
		+ booting * 10
		+ booting
	);
}

void booting_inc(void)
{
	booting++;
}

void init_failed(void)
{
	while (1) fnd_display_number(4444);
}

TASK_WHILE(booting_screen, 2, booting < 10);
TASK_WHILE(booting_inc, MS(500), booting < 10);

void init_succeed(void)
{
	START_TASK(booting_inc);
	START_TASK(booting_screen);
}

void init(void)
{
	buzzer_init();
	fnd_init();
	led_init();
	//switch_init(start_timer, end_timer);
	switch_init(upper_switch, lower_switch);
	timer_init();

	if (!sd_init())
		init_failed();
	init_succeed();
}

/**************************************************** INIT ****************************************************/ 

/**************************************************** DEBUG ****************************************************/ 

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

TASK_DOIF(display, 2, booting >= 10);
TASK_DOIF(mspp, MS(100), booting >= 10 && on);

/**************************************************** DEBUG ****************************************************/ 

int main(void)
{
	init();
	
	START_TASK(display);
	START_TASK(mspp);

	schedule();
	while (1);
}
