#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

#define MOD_TIMER	1
#define INSERT_TO_QUEUE	2
#define SET_OFF		3
#define UNUSED0		4
#define SORT		0b1000 // 8
#define SORT1		0b1001 // 9
#define SORT2		0b1010 // 10
#define SORT3		0b1100 // 12

void debug(int module, int code);

#endif
