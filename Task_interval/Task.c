
//Add some global variables
PRIVILEGED_DATA TCB_t* volatile pxCurrentTCB_copy = NULL;
struct time_interval Time_interval;
unsigned char change_flag = 0;
TickType_t timeold;

/*Back up the current task*/
void pxCurrentTCB_COPY(void)
{
	pxCurrentTCB_copy=pxCurrentTCB; 
}
void judge_whether_calculate(void)
{
	
		if(strcmp(pxCurrentTCB->pcTaskName,pxCurrentTCB_copy->pcTaskName)!=0)
		{
			/*The name is different and the task is going to switch*/
			change_flag=1;
		}
		else{
			change_flag=0;
		}
		
}
       
void calculate_interval(void)
{
	
	if(change_flag==1)
	{
		/*Add the code for calculating*/
	TickType_t timenow;
	timenow=xTaskGetTickCount();
	 Time_interval.interval = timenow - timeold;
	printf("A task switch occurred,Task %s run time=%d, next task %s \r\n",
		pxCurrentTCB_copy->pcTaskName,Time_interval.interval,pxCurrentTCB->pcTaskName);
    timeold=timenow;    
	}
	change_flag=0;

}