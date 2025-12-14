//#define F_CPU	16000000UL

//#include <avr/io.h>
//#include <avr/iom128.h>
//#include <util/delay.h>
//#include <stdint.h>

#include "buzzer.h"
#include "fnd.h"
#include "led.h"
#include "switch.h"
#include "timer.h"
#include "sd.h"
#include "task.h"
#include "file.h"

char is_playing;
int video_cnt;
int video_selected;

void upper_switch();
void lower_switch();

/**************************************************** INIT ****************************************************/ 
char booting = 0;
char booting_led = 0b10101010;

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

void booting_flicker(void)
{
	led_set(booting_led);
	booting_led ^= 0xFF;
}

void init_failed(int error)
{
	while (1) fnd_display_number(error);
}

TASK_WHILE(booting_screen, 2, booting < 10);
TASK_WHILE(booting_inc, MS(300), booting < 10);
TASK_WHILE(booting_flicker, MS(150), booting < 10);

void init_succeed(void)
{
	START_TASK(booting_inc);
	START_TASK(booting_screen);
	START_TASK(booting_flicker);
}

void init(void)
{
	int video_error_code;

	buzzer_init();
	fnd_init();
	led_init();
	//switch_init(start_timer, end_timer);
	switch_init(upper_switch, lower_switch);
	timer_init();

	if (!sd_init())
		init_failed(4444);
	
	video_error_code = fetch_video_list(&video_cnt);
	if (video_error_code)
		init_failed(video_error_code);
	if (video_cnt <= 0)
		init_failed(404);

	init_succeed();
}

/**************************************************** INIT ****************************************************/ 

/**************************************************** MENU ****************************************************/ 

void upper_switch(void)
{
	if (is_playing)
	{

	}
	else
		video_selected = (video_selected + 1) % video_cnt;
}

void lower_switch(void)
{
	
	if (is_playing)
	{

	}
	else
		// change this to "play"
		video_selected = (video_selected + video_cnt - 1) % video_cnt;
}

void show_menu(void)
{
	led_set(1 << (7 - video_selected));
	fnd_display_number_dot((video_selected % 100) * 100 + ((video_cnt - 1) % 100), 1);
}

TASK_DOIF(show_menu, MS(2), booting >= 10);

/**************************************************** MENU ****************************************************/ 

/**************************************************** DEBUG ****************************************************/ 
/*
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
*/
/**************************************************** DEBUG ****************************************************/ 

int main(void)
{
	init();
	
	//START_TASK(display);
	//START_TASK(mspp);
	
	START_TASK(show_menu);

	schedule();
	while (1);
}
