#include <avr/io.h>
#include <avr/iom128.h>

#include "led.h"
#include "debug.h"

void debug(int module, int code)
{
#ifdef DEBUG
	int top, bottom;

	if (code < 0)
		return;
	top = (module & 0b1111) << 4;
	bottom = code & 0b1111;

	led_set(top | bottom);
#else
	return;
#endif
}
