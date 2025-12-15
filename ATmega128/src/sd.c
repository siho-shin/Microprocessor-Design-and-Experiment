#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "sd.h"
#include "spi.h"
#include "task.h"

// DEBUG
#include "fnd.h"

#define SD_CS_LOW()	(PORTB &= ~(1 << SD_CS))
#define SD_CS_HIGH()	(PORTB |= (1 << SD_CS))

volatile char sd_is_init;

void sd_idle_clocks(void)
{
	int i;

	SD_CS_HIGH();
	for (i = 0; i < 1000; i++)
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

	for (i = 0; i < 100; i++)
	{
		ret = spi_transfer(0xFF);
		if (!(ret & 0x80))
			return ret;
	}

	return 0xFE;
}

int sd_comm_init(void)
{
	uint8_t ret;
	uint32_t r7;
	int i;

	sd_idle_clocks();

	SD_CS_LOW();
	ret = sd_send_command(0, 0);
	SD_CS_HIGH();

	if (ret != 0x01)
		return 0;

	do
	{
		SD_CS_LOW();
		ret = sd_send_command(0x08, 0x000001AA);
		SD_CS_HIGH();
	} while (ret != 0x01);

	SD_CS_LOW();
	for (i = 0, r7 = 0; i < 4; i++)
		r7 = (r7 << 8) | spi_transfer(0xFF);
	SD_CS_HIGH();

	do
	{
		SD_CS_LOW();
		ret = sd_send_command(55, 0);
		SD_CS_HIGH();
		spi_transfer(0xFF);

		SD_CS_LOW();
		ret = sd_send_command(41, 0x40000000);
		SD_CS_HIGH();
		spi_transfer(0xFF);
	} while (ret != 0x00);

	return 1;
}

uint8_t sd_read_blk(uint32_t lba, uint8_t *buf)
{
	uint8_t ret;
	int i;

	SD_CS_LOW();

	ret = sd_send_command(17, lba);
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

int async_i;
uint8_t *async_buf;
void (*async_finish)(void);

int async_part(void)
{
	if (async_i == 1)
		emergency_halt(1000);
	async_buf[async_i] = spi_transfer(0xFF);
	async_i++;

	if (async_i >= 512)
		return ASYNC_FINISHED;
	return ASYNC_UNFINISHED;
}

void async_part_task(void)
{
	if (async_part() == ASYNC_UNFINISHED)
	{
		timer_notify(SD_TASK_PERIOD, async_part_task);
		//timer_notify(500, async_part_task);
		return;
	}

	spi_transfer(0xFF);
	spi_transfer(0xFF);

	SD_CS_HIGH();
	spi_transfer(0xFF);

	async_finish();
}

uint8_t sd_read_blk_async(uint32_t lba, uint8_t *buf, async_finished_routine froutine)
{
	uint8_t ret;

	SD_CS_LOW();

	ret = sd_send_command(17, lba);
	if (ret != 0x00)
	{
		SD_CS_HIGH();
		return 1;
	}

	while (spi_transfer(0xFF) != 0xFE);

	async_i = 0;
	async_buf = buf;
	async_finish = froutine;
	async_part_task();
	return 0;
}

int sd_init(void)
{
	if (sd_is_init)
		return 1;

	DDRB |= (1 << SD_CS) | (1 << SD_SCK) | (1 << SD_MOSI);
	DDRB &= ~(1 << SD_MISO);
	SPCR = (1 << SPE) | (1 << MSTR);
	SPCR &= ~((1 << SPR1) | (1 << SPR0));
	SPSR |= (1 << SPI2X);
	sd_is_init = 1;

	return sd_comm_init();
}
