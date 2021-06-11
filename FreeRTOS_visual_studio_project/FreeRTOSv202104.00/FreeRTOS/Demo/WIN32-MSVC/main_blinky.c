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
  * The queue send task is implemented by the task2() function in
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
  * The queue receive task is implemented by the task1() function
  * in this file.  task1() waits for data to arrive on the queue.
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
#include "calculate_tick.h"

static void task1(void* pvParameters);
static void task2(void* pvParameters);
static void task3(void* pvParameters);

void delay_bymyself(int number);

//计算tick的结构体
/*



/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky(void)
{
	
	
		
		
		xTaskCreate(task1,"TASK1", configMINIMAL_STACK_SIZE, NULL,1,NULL);							

		xTaskCreate(task2, "TASK2", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

		xTaskCreate(task3, "TASK3", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
		
		vTaskStartScheduler();
		

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for (;; );
}
/*-----------------------------------------------------------*/





static void task1(void* pvParameters)
{

	/* Prevent the compiler warning about the unused parameter. */
	(void)pvParameters;


	for (;; )
	{
	
		delay_bymyself(300);
		 vTaskDelay(600); 
		
	}
}
/*-----------------------------------------------------------*/

static void task2(void* pvParameters)
{

	/* Prevent the compiler warning about the unused parameter. */
	(void)pvParameters;

	for (;; )
	{
		
		delay_bymyself(400);
		vTaskDelay(700);

	}
}
/*-----------------------------------------------------------*/


static void task3(void* pvParameters)
{
	
	uint8_t pcWriteBuffer[500];
	TickType_t nums=0;

	while (1)
	{
	
		if (nums == 10)
		{

			vTaskGetRunTimeStats((char*)&pcWriteBuffer);
			printf("\r\n任务名       运行计数         使用率\r\n");
			printf("%s\r\n", pcWriteBuffer);
			nums = 0;
		}
		nums++;
	
		delay_bymyself(500);
		vTaskDelay(800);
		
	}
}


void delay_bymyself(int number)
{
	while ((number--) > 0)
		for (int i = 0; i < 3000000; i++); 

}