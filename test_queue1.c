/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

 /******************************************************************************
  * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
  * do not expect to get real time behaviour from the FreeRTOS Windows port, or
  * this demo application.  Also, the timing information in the FreeRTOS+Trace
  * logs have no meaningful units.  See the documentation page for the Windows
  * port for further information:
  * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
  *
  * NOTE 2:  This project provides two demo applications.  A simple blinky style
  * project, and a more comprehensive test and demo application.  The
  * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
  * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
  * in main.c.  This file implements the simply blinky version.  Console output
  * is used in place of the normal LED toggling.
  *
  * NOTE 3:  This file only contains the source code that is specific to the
  * basic demo.  Generic functions, such FreeRTOS hook functions, are defined
  * in main.c.
  ******************************************************************************
  *
  * main_blinky() creates one queue, one software timer, and two tasks.  It then
  * starts the scheduler.
  *
  * The Queue Send Task:
  * The queue send task is implemented by the prvQueueSendTask() function in
  * this file.  It uses vTaskDelayUntil() to create a periodic task that sends
  * the value 100 to the queue every 200 milliseconds (please read the notes
  * above regarding the accuracy of timing under Windows).
  *
  * The Queue Send Software Timer:
  * The timer is a one-shot timer that is reset by a key press.  The timer's
  * period is set to two seconds - if the timer expires then its callback
  * function writes the value 200 to the queue.  The callback function is
  * implemented by prvQueueSendTimerCallback() within this file.
  *
  * The Queue Receive Task:
  * The queue receive task is implemented by the prvQueueReceiveTask() function
  * in this file.  prvQueueReceiveTask() waits for data to arrive on the queue.
  * When data is received, the task checks the value of the data, then outputs a
  * message to indicate if the data came from the queue send task or the queue
  * send software timer.
  *
  * Expected Behaviour:
  * - The queue send task writes to the queue every 200ms, so every 200ms the
  *   queue receive task will output a message indicating that data was received
  *   on the queue from the queue send task.
  * - The queue send software timer has a period of two seconds, and is reset
  *   each time a key is pressed.  So if two seconds expire without a key being
  *   pressed then the queue receive task will output a message indicating that
  *   data was received on the queue from the queue send software timer.
  *
  * NOTE:  Console input and output relies on Windows system calls, which can
  * interfere with the execution of the FreeRTOS Windows port.  This demo only
  * uses Windows system call occasionally.  Heavier use of Windows system calls
  * can crash the port.
  */

  /* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 2000UL )
#define mainTIMER_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 1000UL )

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH					( 2 )

/* The values sent to the queue receive task from the queue send task and the
queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK			( 100UL )
#define mainVALUE_SENT_FROM_TIMER			( 200UL )

/*-----------------------------------------------------------*/
//extern struct time_interval Time_interval;
/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask(void* pvParameters);
static void prvQueueSendTask(void* pvParameters);
//声明计时task
static void Get_timeTask(void* pvParameters);

/*
 * The callback function executed when the software timer expires.
 */
static void prvQueueSendTimerCallback(TimerHandle_t xTimerHandle);
//声明两个定时器中断函数 
static void prvQueueSendTimerCallback1(TimerHandle_t xTimerHandle);
static void prvQueueSendTimerCallback2(TimerHandle_t xTimerHandle);
void delay_bymyself(int number);

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

/*自己实验定义的两个定时器*/
static TimerHandle_t xTimer1 = NULL;
static TimerHandle_t xTimer2 = NULL;
//计算tick的结构体
extern struct time_interva Time_interval;
/*



/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky(void)
{

	const TickType_t xTimerPeriod = mainTIMER_SEND_FREQUENCY_MS; //1s

		/* Create the queue. */
	xQueue = xQueueCreate(1, sizeof(uint32_t));

	if (xQueue != NULL)
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */
		//Rx任务比Tx任务高一个优先级 IDLE+2
		xTaskCreate(prvQueueReceiveTask,			
		"Rx", 							
			configMINIMAL_STACK_SIZE, 		
			NULL, 							
			mainQUEUE_RECEIVE_TASK_PRIORITY,
			NULL);							

		xTaskCreate(prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

		//xTaskCreate(Get_timeTask, "Get_time", configMINIMAL_STACK_SIZE, NULL, (mainQUEUE_RECEIVE_TASK_PRIORITY + 1), NULL);




		
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for (;; );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask(void* pvParameters)
{

	//printf("get in Tx task\r\n");
	TickType_t xNextWakeTime;
	const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS; //2s
	const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK; //发送100

		/* Prevent the compiler warning about the unused parameter. */
	(void)pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for (;; )
	{
		Time_interval.xGetin_Time = xTaskGetTickCount();
		Time_interval.name = "Tx";
		printf("进入发送任务循环，并调用发送函数 当前时间：%d\r\n", Time_interval.xGetin_Time);
		
		//将此任务置于阻塞状态200ms，当处于Blocked状态时，该任务不会消耗任何CPU时间*/
		//vTaskDelayUntil(&xNextWakeTime, xBlockTime);
		//delay_bymyself(500);
		//printf("back in Tx task after task delayuntil 2000ms \r\n");

		//0U代表阻塞时间为0 至少要在0前加一个空格*/
		
		xQueueSend(xQueue, &ulValueToSend, 1000U);
		vTaskDelay(1000);
		//xQueueSend(xQueue, &ulValueToSend, 0U);
		
	}
}
/*-----------------------------------------------------------*/



static void prvQueueReceiveTask(void* pvParameters)
{

	printf("get in Rx task\r\n");
	uint32_t ulReceivedValue;
	/* Prevent the compiler warning about the unused parameter. */
	(void)pvParameters;

	for (;; )
	{

		
		Time_interval.xGetin_Time = xTaskGetTickCount();
		Time_interval.name = "Rx";
	//	printf("back in Rx task before block\r\n");
		xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);
		// xQueueReceive(xQueue, &ulReceivedValue, 5000);
		//delay_bymyself(800);
		 vTaskDelay(6000); //延时一下 让发送队列能满
		//printf("back in Rx task after block and delay_bymyself(800\r\n");

		if (ulReceivedValue == mainVALUE_SENT_FROM_TASK)
		{
			printf("Message received from task\r\n");
		}
		
	}
}
/*-----------------------------------------------------------*/



static void Get_timeTask(void* pvParameters)
{
	printf("get in Get_time task\r\n");
	uint8_t pcWriteBuffer[500];
	TickType_t xNextWakeTime1;

	while (1)
	{

	
		printf("back in Get_time task before delay\r\n");
		Time_interval.xGetin_Time = xTaskGetTickCount();
		Time_interval.name = "Get_time";
		vTaskGetRunTimeStats((char*)&pcWriteBuffer);
		printf("\r\n任务名       运行计数         使用率\r\n");
		printf("%s\r\n", pcWriteBuffer);
		//vTaskDelayUntil(&xNextWakeTime1, xBlockTime1); //s
		vTaskDelay(1500);
		//delay_bymyself(1000);
		printf("back in Get_time task delay 1500 and delay_bymyself(1000)\r\n");
	}
}


void delay_bymyself(int number)
{
	while ((number--) > 0)
		for (int i = 0; i < 3000000; i++); //number=1000 大概延时4s

}

//修改的send函数
/* 
BaseType_t xQueueGenericSend( QueueHandle_t xQueue,
                              const void * const pvItemToQueue,
                              TickType_t xTicksToWait,
                              const BaseType_t xCopyPosition )
{
    int time_full = xTaskGetTickCount();
    //printf("调用发送函数,当前时间%d\r\n",time_full);

    BaseType_t xEntryTimeSet = pdFALSE, xYieldRequired;
    TimeOut_t xTimeOut;
    Queue_t * const pxQueue = xQueue;

    configASSERT( pxQueue );
    configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( UBaseType_t ) 0U ) ) );
    configASSERT( !( ( xCopyPosition == queueOVERWRITE ) && ( pxQueue->uxLength != 1 ) ) );
    #if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
        {
            configASSERT( !( ( xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED ) && ( xTicksToWait != 0 ) ) );
        }
    #endif

    /*lint -save -e904 This function relaxes the coding standard somewhat to
     * allow return statements within the function itself.  This is done in the
     * interest of execution time efficiency. */
    for( ; ; )
    {
        taskENTER_CRITICAL();
        {
            /* Is there room on the queue now?  The running task must be the
             * highest priority task wanting to access the queue.  If the head item
             * in the queue is to be overwritten then it does not matter if the
             * queue is full. */
            if( ( pxQueue->uxMessagesWaiting < pxQueue->uxLength ) || ( xCopyPosition == queueOVERWRITE ) )
            {
                traceQUEUE_SEND( pxQueue );

                #if ( configUSE_QUEUE_SETS == 1 )
                    {
                        const UBaseType_t uxPreviousMessagesWaiting = pxQueue->uxMessagesWaiting;

                        xYieldRequired = prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

                        if( pxQueue->pxQueueSetContainer != NULL )
                        {
                            if( ( xCopyPosition == queueOVERWRITE ) && ( uxPreviousMessagesWaiting != ( UBaseType_t ) 0 ) )
                            {
                                /* Do not notify the queue set as an existing item
                                 * was overwritten in the queue so the number of items
                                 * in the queue has not changed. */
                                mtCOVERAGE_TEST_MARKER();
                            }
                            else if( prvNotifyQueueSetContainer( pxQueue ) != pdFALSE )
                            {
                                /* The queue is a member of a queue set, and posting
                                 * to the queue set caused a higher priority task to
                                 * unblock. A context switch is required. */
                                queueYIELD_IF_USING_PREEMPTION();
                            }
                            else
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                        else
                        {
                            /* If there was a task waiting for data to arrive on the
                             * queue then unblock it now. */
                            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                            {
                                if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                                {
                                    /* The unblocked task has a priority higher than
                                     * our own so yield immediately.  Yes it is ok to
                                     * do this from within the critical section - the
                                     * kernel takes care of that. */
                                    queueYIELD_IF_USING_PREEMPTION();
                                }
                                else
                                {
                                    mtCOVERAGE_TEST_MARKER();
                                }
                            }
                            else if( xYieldRequired != pdFALSE )
                            {
                                /* This path is a special case that will only get
                                 * executed if the task was holding multiple mutexes
                                 * and the mutexes were given back in an order that is
                                 * different to that in which they were taken. */
                                queueYIELD_IF_USING_PREEMPTION();
                            }
                            else
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                    }
                #else /* configUSE_QUEUE_SETS */
                    {
                        xYieldRequired = prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

                        /* If there was a task waiting for data to arrive on the
                         * queue then unblock it now. */
                        if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                        {
                            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                            {
                                /* The unblocked task has a priority higher than
                                 * our own so yield immediately.  Yes it is ok to do
                                 * this from within the critical section - the kernel
                                 * takes care of that. */
                                queueYIELD_IF_USING_PREEMPTION();
                            }
                            else
                            {
                                mtCOVERAGE_TEST_MARKER();
                            }
                        }
                        else if( xYieldRequired != pdFALSE )
                        {
                            /* This path is a special case that will only get
                             * executed if the task was holding multiple mutexes and
                             * the mutexes were given back in an order that is
                             * different to that in which they were taken. */
                            queueYIELD_IF_USING_PREEMPTION();
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                #endif /* configUSE_QUEUE_SETS */

                taskEXIT_CRITICAL();
                printf("发送成功\r\n");
                return pdPASS;
            }
            else
            {
                if( xTicksToWait == ( TickType_t ) 0 )
                {
                    /* The queue was full and no block time is specified (or
                     * the block time has expired) so leave now. */
                    taskEXIT_CRITICAL();

                    /* Return to the original privilege level before exiting
                     * the function. */
                    traceQUEUE_SEND_FAILED( pxQueue );
                    return errQUEUE_FULL;
                }
                else if( xEntryTimeSet == pdFALSE )
                {
                  //  printf("队列已满，设置阻塞时间，当前时间为%d\r\n", time_full);
                    /* The queue was full and a block time was specified so
                     * configure the timeout structure. */
                    vTaskInternalSetTimeOutState( &xTimeOut );
                    xEntryTimeSet = pdTRUE;
                }
                else
                {
                    /* Entry time was already set. */
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        }
        taskEXIT_CRITICAL();

        /* Interrupts and other tasks can send to and receive from the queue
         * now the critical section has been exited. */
        printf("队列已满，堵塞时间还没到 当前时间为%d，挂起任务调度器\r\n", time_full);
        vTaskSuspendAll();
        prvLockQueue( pxQueue );
        //任务调度器上锁， 代码执行到这里说明当前的状况是队列已满了，而且设置了不为 0的阻塞时间。
        
      
        /* Update the timeout state to see if it has expired yet. */
        if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
        {
            if( prvIsQueueFull( pxQueue ) != pdFALSE )
            {
                traceBLOCKING_ON_QUEUE_SEND( pxQueue );
                vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), xTicksToWait );

                /* Unlocking the queue means queue events can effect the
                 * event list.  It is possible that interrupts occurring now
                 * remove this task from the event list again - but as the
                 * scheduler is suspended the task will go onto the pending
                 * ready last instead of the actual ready list. */
                prvUnlockQueue( pxQueue );
                printf("将任务添加到队列的 xTasksWaitingToSend 列表,恢复调度\r\n");
                /* Resuming the scheduler will move tasks from the pending
                 * ready list into the ready list - so it is feasible that this
                 * task is already in a ready list before it yields - in which
                 * case the yield will not cause a context switch unless there
                 * is also a higher priority task in the pending ready list. */
                if( xTaskResumeAll() == pdFALSE )
                {
                    portYIELD_WITHIN_API();
                }
            }
            else
            {
                /* Try again. */
                time_full = xTaskGetTickCount();
                printf("堵塞时间还没到，且队列有空闲项，重试一次,恢复调度，当前时间为%d\r\n", time_full);
                prvUnlockQueue( pxQueue );
                ( void ) xTaskResumeAll();
            }
        }
        else
        {
            /* The timeout has expired. */
            printf("堵塞时间到了，恢复调度，当前时间为%d\r\n", time_full);
            prvUnlockQueue( pxQueue );
            ( void ) xTaskResumeAll();

            traceQUEUE_SEND_FAILED( pxQueue );
            return errQUEUE_FULL;
        }
    } /*lint -restore */
}
*/