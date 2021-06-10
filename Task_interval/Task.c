#include "calculate_tick.h"

//Add some global variables
PRIVILEGED_DATA TCB_t* volatile pxCurrentTCB_copy = NULL;


/*Back up the current task*/
void pxCurrentTCB_COPY(void)
{
	pxCurrentTCB_copy=pxCurrentTCB; 
}
void judge_and_calculate(void)
{
	
		
		if(strcmp(pxCurrentTCB->pcTaskName,pxCurrentTCB_copy->pcTaskName)!=0)
		{
			TickType_t timenow;
			timenow=xTaskGetTickCount();
			Time_interval.interval = timenow - timeold;
			printf("A task switch occurred,Task %s run time=%d, next task %s \r\n",
			pxCurrentTCB_copy->pcTaskName,Time_interval.interval,pxCurrentTCB->pcTaskName);
			timeold =timenow;
		}
		
		
}
       
