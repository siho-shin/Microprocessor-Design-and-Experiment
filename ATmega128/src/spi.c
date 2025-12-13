#include <avr/io.h>
#include <avr/iom128.h>

#include "spi.h"

uint8_t spi_transfer(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}
