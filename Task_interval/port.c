/*Add two functions to the original */
__asm void xPortPendSVHandler(void)
{
	extern uxCriticalNesting;
	extern pxCurrentTCB;
	extern vTaskSwitchContext;
	extern pxCurrentTCB_COPY;
	extern judge_and_calculate;

	PRESERVE8

	mrs r0, psp isb

				ldr r3,
		= pxCurrentTCB /* Get the location of the current TCB. */
			ldr r2,
		[r3]

		stmdb r0 !,
		{r4 - r11} /* Save the remaining registers. */
	str r0,
		[r2] /* Save the new top of stack into the first member of the TCB. */

		stmdb sp !,
		{r3, r14} mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY msr basepri, r0 dsb isb bl pxCurrentTCB_COPY bl vTaskSwitchContext bl judge_and_calculate /*Executes the function to calculate the interval output*/
																				 mov r0,
		#0 msr basepri, r0 ldmia sp !, {r3, r14}

	ldr r1,
		[r3] ldr r0, [r1] /* The first item in pxCurrentTCB is the task top of stack. */
		ldmia r0 !,
		{r4 - r11} /* Pop the registers and the critical nesting count. */
	msr psp,
		r0
			isb
				bx r14
					nop
}

void update_deadline_systick(void)
{
	for (int i = 0; i < task_numbers; i++)
	{
		deadline_systick[i] += 1;
		/*If a task has not been run at least once within its corresponding task_deadline*/
		if (deadline_systick[i] > Time_interval.task_deadline[i])
		{
			printf("The task  %d was not carried out within the deadline %d\r\n", i, Time_interval.task_deadline[i]);
			deadline_systick[i] = 0; //Reset to 0 after timeout
		}
	}
}
/*Add a function to the systick interrupt function*/
void xPortSysTickHandler(void)
{
	vPortRaiseBASEPRI();
	{
		/* Increment the RTOS tick. */
		if (xTaskIncrementTick() != pdFALSE)
		{
			/*Call the function we defined,*/
			update_deadline_systick();
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}
	vPortClearBASEPRIFromISR();
}