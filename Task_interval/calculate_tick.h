
#ifndef CALCULATE_TICK_H
#define CALCULATE_TICK_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define task_numbers 6  //task1~3 and Tmrsvc and IDLE task
typedef struct time_interval
{
	unsigned int  interval; //time interval
	unsigned int task_deadline[task_numbers]; /* Each task should run its corresponding task_deadline at least once per interval */
	unsigned int Max_runtime[task_numbers];/*Each task must run for no longer than this value */
};

struct time_interval Time_interval;
unsigned int  timeold ;
unsigned int deadline_systick[task_numbers];/*Stats how long it has been since each task was last run (compared to task_deadline)*/

#endif