#ifndef __SD_H__
#define __SD_H__

#include <avr/io.h>
#include <avr/iom128.h>

#define SD_CS	(PB0)
#define SD_SCK	(PB1)
#define SD_MOSI	(PB2)
#define SD_MISO	(PB3)

// returns 0 is init has failed
int sd_init(void);
uint8_t sd_read_blk(uint32_t lba, uint8_t *buf);

#endif
