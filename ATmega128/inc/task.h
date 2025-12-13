#ifndef __TASK_H__
#define __TASK_H__

#include "timer.h"

#define TASK_NO_CONDITION	1

#define TASK(func, period, condition)	\
	void func##_task(void)		\
	{				\
		if (!condition)		\
			return;		\
		func();			\
		timer_notify(period, func##_task);	\
	}

#define START_TASK(func)	\
	func##_task();
		

#endif
