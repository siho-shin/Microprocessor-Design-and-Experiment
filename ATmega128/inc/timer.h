#ifndef __TIMER_H__
#define __TIMER_H__

#define WAITING_QUEUE_INVALID	0x7FFFFFFF
#define WAITING_QUEUE_SZ	32

#define MS_TO_UNITS(x)		((long long)((x) * 4))

typedef void (*timerfunc_t)(void);
typedef unsigned long long time_t;

void timer_init(void);
void timer_notify(time_t ms, timerfunc_t func);
void schedule(void);

#endif
