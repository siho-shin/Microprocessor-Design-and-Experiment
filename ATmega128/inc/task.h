#ifndef __TASK_H__
#define __TASK_H__

#include "timer.h"

#define TASK_WHILE_TRUE	1

#define TASK_WHILE(func, period, condition)		\
	void __##func##_task(void)			\
	{						\
		if (!(condition))			\
			return;				\
		func();					\
		timer_notify((period), __##func##_task);\
	}

#define TASK_DOIF(func, period, start_cond)		\
	void __##func##_task(void)			\
	{						\
		timer_notify((period), __##func##_task);\
		if ((start_cond))			\
			func();				\
	}

#define TASK_DYNAMIC(func, period, start_cond, end_cond)\
	void __##func##_task(void)			\
	{						\
		if ((end_cond))				\
			return;				\
		timer_notify((period), __##func##_task);\
		if ((start_cond))			\
			func();				\
	}

#define START_TASK(func)	\
	__##func##_task();
		

#endif
