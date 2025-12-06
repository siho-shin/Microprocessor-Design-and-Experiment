#ifndef __JOBS_H__
#define __JOBS_H__

typedef struct
{
	char job_name;
	int execution_time;
	int deadline;
	void (*job_func)(void);
}	job_t, *pjob_t;

#endif
