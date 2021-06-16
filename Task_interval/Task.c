#include "calculate_tick.h"

//Add some global variables
PRIVILEGED_DATA TCB_t *volatile pxCurrentTCB_copy = NULL;

/*Back up the current task*/
void pxCurrentTCB_COPY(void)
{
	/* Set the deadline_systick of this task to 0*/
	deadline_systick[pxCurrentTCB_copy->uxTCBNumber] = 0;
	/*copy this tcb*/
	pxCurrentTCB_copy = pxCurrentTCB;
}
void judge_and_calculate(void)
{

	if (strcmp(pxCurrentTCB->pcTaskName, pxCurrentTCB_copy->pcTaskName) != 0)
	{
		TickType_t timenow;
		timenow = xTaskGetTickCount();
		Time_interval.interval = timenow - timeold;
		/*If a task runs out of time，print some information*/
		if (Time_interval.interval > Time_interval.Max_runtime[(pxCurrentTCB_copy->uxTCBNumber)])
		{
			printf("The task % s has run out of time % d,limit is %d 	\r\n",
				   pxCurrentTCB_copy->pcTaskName, Time_interval.interval, Time_interval.Max_runtime[(pxCurrentTCB_copy->uxTCBNumber)]);
		}
		/*
			printf("A task switch occurred,Task %s run time=%d, next task %s \r\n",
			pxCurrentTCB_copy->pcTaskName,Time_interval.interval,pxCurrentTCB->pcTaskName); */
		timeold = timenow;
	}
}
