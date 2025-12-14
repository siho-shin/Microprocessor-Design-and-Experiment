#ifndef __FND_H__
#define __FND_H__

#include <stdint.h>
#include "timer.h"

#define FND_DISPLAY_RATE	TICKS(8)

void fnd_init(void);
void fnd_display_number(uint16_t number);
void fnd_display_one_number(uint16_t number, int pos);
void fnd_display_number_dot(uint16_t number, int dotpos);

#endif
