#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "sd.h"

#define SD_CS_LOW()	(PORTB &= ~(1 << SD_CS))
#define SD_CS_HIGH()	(PORTB |= (1 << SD_CS))

uint8_t spi_transfer(uint8_t data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}

void sd_idle_clocks(void)
{
	int i;

	SD_CS_HIGH();

	for (i = 0; i < 10; i++)
		spi_transfer(0xFF);
}

uint8_t sd_send_command(uint8_t cmd, uint32_t arg)
{
	uint8_t ret;
	int i;

	spi_transfer(0xFF);
	spi_transfer(0x40 | cmd);
	spi_transfer(arg >> 24);
	spi_transfer(arg >> 16);
	spi_transfer(arg >> 8);
	spi_transfer(arg);

	if (cmd == 0)
		spi_transfer(0x95);
	else if (cmd == 8)
		spi_transfer(0x87);
	else
		spi_transfer(0xFF);

	for (i = 0; i < 10; i++)
	{
		ret = spi_transfer(0xFF);
		if (!(ret & 0x80))
			return ret;
	}

	return 0xFF;
}

int sd_comm_init(void)
{
	uint8_t ret;

	sd_idle_clocks();

	SD_CS_LOW();
	ret = sd_send_command(0, 0);
	SD_CS_HIGH();

	if (ret != 0x01)
		return 0;

	do
	{
		SD_CS_LOW();
		ret = sd_send_command(1, 0);
		SD_CS_HIGH();
	} while (ret != 0x00);

	return 1;
}

uint8_t sd_read_blk(uint32_t lba, uint8_t *buf)
{
	uint8_t ret;
	int i;

	SD_CS_LOW();

	ret = sd_send_command(17, lba * 512);
	if (ret != 0x00)
	{
		SD_CS_HIGH();
		return ret;
	}

	while (spi_transfer(0xFF) != 0xFE);

	for (i = 0; i < 512; i++)
		buf[i] = spi_transfer(0xFF);

	spi_transfer(0xFF);
	spi_transfer(0xFF);

	SD_CS_HIGH();
	spi_transfer(0xFF);

	return 0;
}

int sd_init(void)
{
	DDRF |= (1 << SD_CS) | (1 << SD_SCK) | (1 << SD_MOSI);
	DDRF &= ~(1 << SD_MISO);

	SPCR = (1 << SPE) | (1 << MSTR);
	SPCR |= (1 << SPR1) | (1 << SPR0);

	return sd_comm_init();
}
