#ifndef __TASK_H__
#define __TASK_H__

#include "timer.h"

#define TASK_WHILE_TRUE		1

#define ASYNC_UNFINISHED	0
#define ASYNC_FINISHED		1

typedef void (*async_finished_routine)(void);

#define TASK_WHILE(func, period, condition)		\
	char __##func##_stopper = 0;			\
	void __##func##_task(void)			\
	{						\
		if (__##func##_stopper)			\
			return;				\
		if (!(condition))			\
			return;				\
		func();					\
		timer_notify((period), __##func##_task);\
	}

#define TASK_DOIF(func, period, start_cond)		\
	char __##func##_stopper = 0;			\
	void __##func##_task(void)			\
	{						\
		if (__##func##_stopper)			\
			return;				\
		timer_notify((period), __##func##_task);\
		if ((start_cond))			\
			func();				\
	}

#define TASK_DYNAMIC(func, period, condition, end_cond)	\
	char __##func##_stopper = 0;			\
	void __##func##_task(void)			\
	{						\
		if (__##func##_stopper)			\
			return;				\
		if ((end_cond))				\
			return;				\
		timer_notify((period), __##func##_task);\
		if ((condition))			\
			func();				\
	}

#define TASK_WHILE_ASYNC(call, period, finish_routine)	\
	char __##func##_stopper = 0;			\
	void __##func##_task(void)			\
	{						\
		if (__##func##stopper)			\
			return;				\
		if (##call##)				\
		{					\
			finish_routine();		\
			return;				\
		}					\
		timer_notify((period), __##func##_task);\
	}

#define START_TASK(func)	\
	__##func##_stopper = 0;	\
	__##func##_task();

#define STOP_TASK(func)		\
	__##func##_stopper = 1;	\

#endif
