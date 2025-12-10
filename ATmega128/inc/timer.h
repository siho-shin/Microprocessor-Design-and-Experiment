#ifndef __TIMER_H__
#define __TIMER_H__

#define WAITING_QUEUE_INVALID	0x7FFF
#define WAITING_QUEUE_SZ	32

typedef void (*timerfunc_t)(void);

void timer_init(void);
void timer_sleep(int ms);
int timer_notify(int ms, timerfunc_t func);

#endif
