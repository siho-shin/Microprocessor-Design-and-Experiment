#ifndef __SD_H__
#define __SD_H__

#include <avr/io.h>
#include <avr/iom128.h>

#include "timer.h"
#include "task.h"

#define SD_ERROR_CODE	4444

#define SD_TASK_PERIOD	TICKS(3)

#define SD_CS	(PB0)
#define SD_SCK	(PB1)
#define SD_MOSI	(PB2)
#define SD_MISO	(PB3)

// returns 0 is init has failed
int sd_init(void);
uint8_t sd_read_blk(uint32_t lba, uint8_t *buf);
uint8_t sd_read_blk_async(uint32_t lba, uint8_t *buf, async_finished_routine froutine);

#endif
