#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <util/delay.h>
#include <stdint.h>

#include "fnd.h"

const uint8_t fnd_number[10] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66,
	0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

const uint8_t fnd_select[4] = {
	0x08, 0x04, 0x02, 0x01
};

volatile char fnd_is_init;

void fnd_init(void)
{
	if (fnd_is_init)
		return;

	DDRC |= 0xFF;
	DDRG |= 0x0F;

	fnd_is_init = 1;
}

void fnd_display_number(uint16_t number)
{
	fnd_display_number_dot(number, -1);
}

void fnd_display_one_number(uint16_t number, int pos)
{
	PORTC = fnd_number[number];
	PORTG = fnd_select[pos];
}

void fnd_display_number_dot(uint16_t number, int dotpos)
{
	int i;
	int ten;
	uint8_t fnd[4] = { 0, };

	for (i = 3, ten = 1; i >= 0; i--, ten *= 10)
		fnd[i] = (number / ten) % 10;

	for (i = 0; i < 4; i++)
	{
		PORTC = fnd_number[fnd[i]] | (i == dotpos ? 0x80 : 0x00);
		PORTG = fnd_select[i];
		_delay_ms(2);
	}
}
