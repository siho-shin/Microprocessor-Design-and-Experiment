#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "switch.h"

switch_handler_t handlers[2];

ISR(INT4_vect)
{
	if (handlers[0])
		handlers[0]();
}

ISR(INT5_vect)
{
	if (handlers[1])
		handlers[1]();
}

void switch_init(switch_handler_t handler0, switch_handler_t handler1)
{
	DDRA |= 0xFF;
	DDRG |= 0x00;
	EICRB |= 0xAA;
	EIMSK |= 0x30;
	SREG |= 0x80;

	handlers[0] = handler0;
	handlers[1] = handler1;
}

switch_handler_t switch_change_handler(int index, switch_handler_t new_handler)
{
	switch_handler_t old_handler;

	if (index < 0 || index >= 2)
		return (switch_handler_t)0;

	old_handler = handlers[index];
	handlers[index] = new_handler;

	return old_handler;
}
