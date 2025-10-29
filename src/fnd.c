#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <util/delay.h>
#include <stdint.h>

#include "fnd.h"

uint8_t fnd_number[10] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66,
	0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

uint8_t fnd_select[4] = {
	0x08, 0x04, 0x02, 0x01
};

void fnd_init(void)
{
	DDRC |= 0xFF;
	DDRG |= 0x0F;
}

void display_and_delay(int on_count)
{
	int i;

	for (i = on_count; i <= 4; i++)
		_delay_ms(5);
}

void fnd_display_number(uint16_t number)
{
	int i;
	int ten;
	uint8_t fnd[4] = { 0, };
	int on_count = 0;

	for (i = 3, ten = 10; i >= 0; i--, ten *= 10)
		fnd[i] = (number / ten) % 10;
	for (i = 0; i < 4; i++)
		if (fnd[i] != 0)
			break;
	on_count = 4 - i;

	if (on_count == 0)
		return;
	for (i = 0; i < on_count; i++)
	{
		int index = i + 4 - on_count;

		PORTC = fnd_number[fnd[index]];
		PORTG = fnd_select[index];
		display_and_delay(on_count);
	}
}
