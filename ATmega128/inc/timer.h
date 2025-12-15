#ifndef __TIMER_H__
#define __TIMER_H__

#define WAITING_QUEUE_INVALID	0x7FFFFFFF
#define WAITING_QUEUE_SZ	32

#define TICKS_PER_SEC		16000

typedef void (*timerfunc_t)(void);
typedef unsigned long long time_t;

#define TICKS(x)	((time_t)(x))
#define MS(x)		((time_t)(((x) * (TICKS_PER_SEC / 1000))))

void timer_init(void);
void timer_notify(time_t ms, timerfunc_t func);
void schedule(void);
void emergency_halt(int error_code);

#endif
