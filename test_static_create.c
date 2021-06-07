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

#include <stdlib.h>


/* Visual studio intrinsics used so the __debugbreak() function is available
should an assert get hit. */
#include <intrin.h>




/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 200UL )
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

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* A software timer that is started from the tick hook. */
static TimerHandle_t xTimer = NULL;

/*自己实验定义的两个定时器*/
static TimerHandle_t xTimer1 = NULL;
static TimerHandle_t xTimer2 = NULL;


extern struct xSTATIC_TCB xIdleTaskTCB;
extern StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
extern struct xSTATIC_TCB xTimerTaskTCB;
//任务优先级
#define START_TASK_PRIO		1
#define START_STK_SIZE 		128 

//开始任务堆栈
StackType_t StartTaskStack[START_STK_SIZE];
//开始任务TCB
StaticTask_t StartTaskTCB;
//开始任务句柄
TaskHandle_t StartTask_Handler;
void start_task(void* pvParameters);

//ÈÎÎñÓÅÏÈ¼¶
#define TASK1_TASK_PRIO		2
//ÈÎÎñ¶ÑÕ»´óÐ¡	
#define TASK1_STK_SIZE 		128  
//ÈÎÎñ¶ÑÕ»
StackType_t Task1TaskStack[TASK1_STK_SIZE];
//ÈÎÎñ¿ØÖÆ¿é
StaticTask_t Task1TaskTCB;
//ÈÎÎñ¾ä±ú
TaskHandle_t Task1Task_Handler;
//ÈÎÎñº¯Êý
void task1_task(void* pvParameters);

//ÈÎÎñÓÅÏÈ¼¶
#define TASK2_TASK_PRIO		3
//ÈÎÎñ¶ÑÕ»´óÐ¡	
#define TASK2_STK_SIZE 		128 
//ÈÎÎñ¶ÑÕ»
StackType_t Task2TaskStack[TASK2_STK_SIZE];
//ÈÎÎñ¿ØÖÆ¿é
StaticTask_t Task2TaskTCB;
//ÈÎÎñ¾ä±ú
TaskHandle_t Task2Task_Handler;
//ÈÎÎñº¯Êý
void task2_task(void* pvParameters);

void start_task(void* pvParameters)
{
	taskENTER_CRITICAL();

	Task1Task_Handler = xTaskCreateStatic((TaskFunction_t)task1_task,
		(const char*)"task1_task",
		(uint32_t)TASK1_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)TASK1_TASK_PRIO,
		(StackType_t*)Task1TaskStack,
		(StaticTask_t*)&Task1TaskTCB);

	Task2Task_Handler = xTaskCreateStatic((TaskFunction_t)task2_task,
		(const char*)"task2_task",
		(uint32_t)TASK2_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)TASK2_TASK_PRIO,
		(StackType_t*)Task2TaskStack,
		(StaticTask_t*)&Task2TaskTCB);

	printf("delete start task");
	//vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
	vTaskDelete(StartTask_Handler);
	
}

void simple_test(void)
{
	const TickType_t xTimerPeriod = (1000UL); //1s

	
	StartTask_Handler = xTaskCreateStatic((TaskFunction_t)start_task,
		(const char*)"start_task",
		(uint32_t)START_STK_SIZE,
		(void*)NULL,
		(UBaseType_t)START_TASK_PRIO,
		(StackType_t*)StartTaskStack,
		(StaticTask_t*)&StartTaskTCB);

	/* Create the software timer, but don't start it yet. */
	xTimer1 = xTimerCreate("Timer1",				/* The text name assigned to the software timer - for debug only as it is not used by the kernel. */
		xTimerPeriod,		/* 1s周期. */
		pdTRUE,			/* 超时就会触发中断函数. */
		NULL,				/* The timer's ID is not used. */
		prvQueueSendTimerCallback1);/* 定时器中断函数 */

	xTimerStart(xTimer1, 0); /* 0s阻塞时间 */

	xTimer2 = xTimerCreate("Timer2",				/* The text name assigned to the software timer - for debug only as it is not used by the kernel. */
		xTimerPeriod,		/* 1s周期. */
		pdTRUE,			/* 超时就会触发中断函数. */
		NULL,				/* The timer's ID is not used. */
		prvQueueSendTimerCallback2);/* 定时器中断函数 */

	xTimerStart(xTimer2, 0); /* 0s阻塞时间 */

	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	for (;; );
}






static void prvQueueSendTimerCallback1(TimerHandle_t xTimerHandle)
{
	const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;

	/* Avoid compiler warnings resulting from the unused parameter. */
	(void)xTimerHandle;
	printf("timer1Callback\r\n");
}

static void prvQueueSendTimerCallback2(TimerHandle_t xTimerHandle)
{
	const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TIMER;

	/* Avoid compiler warnings resulting from the unused parameter. */
	(void)xTimerHandle;
	printf("timer2Callback\r\n");
}


//中断函数
static void interrupt_task(void* pvParameters)
{
	TickType_t xNextWakeTime;
	const TickType_t xBlockTime = mainTASK_SEND_FREQUENCY_MS; //200ms
	const uint32_t ulValueToSend = mainVALUE_SENT_FROM_TASK; //发送100

		/* Prevent the compiler warning about the unused parameter. */
	(void)pvParameters;
	//记录 进入核 出的时间tick log task 记录时间
	static unsigned int total_num = 0;
	while (1)
	{
		total_num += 1; if (total_num == 5) {
			printf("关闭中断 \r\n");
			portDISABLE_INTERRUPTS(); //关闭中断  
			//delay_xms(5000); //延时 5s  
			printf("打开中断 \r\n"); //打开中断
			portENABLE_INTERRUPTS();
		}
		vTaskDelay(1000);
	}
}


static void Get_timeTask(void* pvParameters)
{
	uint8_t pcWriteBuffer[500];
	TickType_t xNextWakeTime1;
	const TickType_t xBlockTime1 = mainTASK_SEND_FREQUENCY_MS; //200ms
	while (1)
	{

		//vTaskList((char*)&pcWriteBuffer);
		//printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
		//printf("%s\r\n", pcWriteBuffer);

		vTaskGetRunTimeStats((char*)&pcWriteBuffer);
		printf("\r\n任务名       运行计数         使用率\r\n");
		printf("%s\r\n", pcWriteBuffer);

		vTaskDelay(6000);
	}
}

//
void task1_task(void* pvParameters)
{
	int task1_num = 0;

	while (1)
	{
		task1_num++;
		printf("task1num=%d\r\n", task1_num);
		if (task1_num == 5)
		{
			vTaskDelete(Task2Task_Handler);
			printf("delete task2!\r\n");
			
		}
		vTaskDelay(1000);                           //dalay for 1s	
	}
}


void task2_task(void* pvParameters)
{
	int task2_num = 0;

	while (1)
	{
		task2_num++;

		printf("task2_num=%d\r\n", task2_num);
		vTaskDelay(1000);                           //1s
	}
}