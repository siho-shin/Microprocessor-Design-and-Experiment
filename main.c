#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned char data = 0x00;

SIGNAL(INT4_vect)
{
	data ^= 0xF0;
}

SIGNAL(INT5_vect)
{
	data ^= 0x0F;
}

int main(void)
{
	DDRA = 0xFf;
	DDRG = 0x00;
	EICRB = 0xAA;
	EIMSK = 0x30;
	SREG = 0x80;

	while (1)
		PORTA = data;

	return 0;
}
