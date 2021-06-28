

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

TaskHandle_t  handle_task1;
TaskHandle_t handle_task2;
TaskHandle_t handle_task3;
//二值信号量句柄
SemaphoreHandle_t BinarySemaphore; //二值信号量句柄

void delay_bymyself(int number);

//计算tick的结构体
/*

/*-----------------------------------------------------------*/

/*** SEE THE COMMENTS AT THE TOP OF THIS FILE ***/
void main_blinky(void)
{
	
	BinarySemaphore = xSemaphoreCreateBinary();
		
		
	//xTaskCreate(task1,"TASK1", configMINIMAL_STACK_SIZE, NULL,2, &handle_task1);

	//xTaskCreate(task2, "TASK2", configMINIMAL_STACK_SIZE, NULL, 1, &handle_task2);

	xTaskCreate(task3, "TASK3", configMINIMAL_STACK_SIZE, NULL, 3, &handle_task3);
			
		vTaskStartScheduler();
		

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	for (;; );
}
/*-----------------------------------------------------------*/



static void task3(void* pvParameters)
{

	uint8_t pcWriteBuffer[500];
	TickType_t nums=0;
	unsigned char *buffer;
	int a;
	unsigned char times, i, key = 0;
	times = 0;
	TickType_t freemem;
	while (1)
	{
			a = getchar();
			switch (a) {
			case 'a':
				buffer = pvPortMalloc(30); //申请内存， 30个字节 (2) 
				printf("申请到的内存地址为 :%#x\r\n",(int)buffer);
				break;
			case 'b':
				if (buffer != NULL)
				vPortFree(buffer); //释放内存 
        /*
        释放内存以后将 buffer设置为 NULL，函数 vPortFree()释放内存以后并不会将 buffer清零，此时 buffer还是上次申请到的内存地址，如果此时再调用指针 buffer的话你会发现还是
        可以使用的！感觉好像没有释放内存，所以这里将 buffer清零！*/
				buffer=NULL;
				break;

			case 'c':
				if (buffer != NULL) //buffer可用 ,使用 buffer (5) 
				{ 
					times++; 
					sprintf((char*)buffer,"User %d Times",times);//向 buffer中填写一些数据
					printf("buffer=%s\n", buffer);
				}
				break;
				
		}
    /*
    我们申请的是30个字节的
    内存！内存为什么会少 40个字节呢？多余的 10个字节是怎么回事？ 这是应用所需的内存大小上加上结构体 BlockLink_t的大小
    然后做8字节对齐后导致的结果。*/
			freemem = xPortGetFreeHeapSize(); //获取剩余内存大小
			printf("freemem=%d\n", freemem);
			vTaskDelay(100);
		
	}
}

