#ifndef __TIMER_H__
#define __TIMER_H__

#define WAITING_QUEUE_INVALID	0x7FFFFFFF
#define WAITING_QUEUE_SZ	32

typedef void (*timerfunc_t)(void);

void timer_init(void);
void timer_notify(int ms, timerfunc_t func);
void schedule(void);

#endif
