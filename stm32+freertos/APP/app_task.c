#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Motor.h"
#include "PWM.h"
#include "AD.h"
#include "freertos.h"
#include "task.h"
#include "app_getwd.h"

uint8_t RxData, Data;
int8_t speed;
int p = 1;

// 创建启动任务
TaskHandle_t start_task_handle;
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
void App_Task_Start(void *args);

// 创建获取温度的任务
TaskHandle_t getwd_task_handle;
#define GETWD_TASK_STACK 128
#define GETWD_TASK_PRIORITY 2
void App_Task_GetWd(void *args);

/* 发送给串口1的任务 */
TaskHandle_t submit_task_handle;
#define SUBMIT_TASK_STACK 128
#define SUBMIT_TASK_PRIORITY 2
void App_Task_Submit(void *args);

/* 获取给串口2的任务 */
TaskHandle_t receive_task_handle;
#define RECEIVE_TASK_STACK 128
#define RECEIVE_TASK_PRIORITY 2
void App_Task_Receive(void *args);

/* 进行显示 */
TaskHandle_t display_task_handle;
#define DISPLAY_TASK_STACK 128
#define DISPLAY_TASK_PRIORITY 2
void App_Task_Display(void *args);

void App_Task_Init(void)
{
    /* 1. 创建启动任务 */
    xTaskCreate(
        (TaskFunction_t)App_Task_Start,
        (const char *)"App_Task_Start",
        (configSTACK_DEPTH_TYPE)START_TASK_STACK,
        NULL,
        (UBaseType_t)START_TASK_PRIORITY,
        &start_task_handle
    );

    vTaskStartScheduler();
}

void App_Task_Start(void *args)
{
	/*临界区 ， 来防止任务的切换*/
    taskENTER_CRITICAL();
    // 创建其他任务
    xTaskCreate(
        (TaskFunction_t)App_Task_GetWd,
        (const char *)"App_Task_GetWd",
        (configSTACK_DEPTH_TYPE)GETWD_TASK_STACK,
        NULL,
        (UBaseType_t)GETWD_TASK_PRIORITY,
        &getwd_task_handle
    );

    xTaskCreate(
        (TaskFunction_t)App_Task_Submit,
        (const char *)"App_Task_Submit",
        (configSTACK_DEPTH_TYPE)SUBMIT_TASK_STACK,
        NULL,
        (UBaseType_t)SUBMIT_TASK_PRIORITY,
        &submit_task_handle
    );

    xTaskCreate(
        (TaskFunction_t)App_Task_Receive,
        (const char *)"App_Task_Receive",
        (configSTACK_DEPTH_TYPE)RECEIVE_TASK_STACK,
        NULL,
        (UBaseType_t)RECEIVE_TASK_PRIORITY,
        &receive_task_handle
    );

    xTaskCreate(
        (TaskFunction_t)App_Task_Display,
        (const char *)"App_Task_Display",
        (configSTACK_DEPTH_TYPE)DISPLAY_TASK_STACK,
        NULL,
        (UBaseType_t)DISPLAY_TASK_PRIORITY,
        &display_task_handle
    );

    vTaskDelete(NULL);
    taskEXIT_CRITICAL();
}

// 获取温度函数
void App_Task_GetWd(void *args)
{
    while (1)
    {
//		
		taskENTER_CRITICAL();
		Data =  getwd();
		//printf("-- App_Task_GetWd run \r\n");
		taskEXIT_CRITICAL();
//		taskENTER_CRITICAL();
//		printf("-- App_Task_GetWd run \r\n");
//		taskEXIT_CRITICAL();
		vTaskDelay(500);

		
    }
}

// 发送温度给串口1的函数
void App_Task_Submit(void *args)
{
    while (1)
    {
			uint8_t d = 61 , dd = 62;
        //Serial_SendByte(Data);
		taskENTER_CRITICAL();
		if(Data >= 30 && p == 1)
		{
			Serial_SendByte(d);
			p = 0;
		}	
		if(Data < 30 && p == 0)
		{
			Serial_SendByte(dd);
			p = 1;
		}
			
		//uint8_t d = 63;
		//OLED_ShowNum(5,2,Data,4);
		//printf("-- App_Task_Submit run \r\n");
		taskEXIT_CRITICAL();
        vTaskDelay(500);

    }
}

// 接收串口1的数据并控制电机
void App_Task_Receive(void *args)
{
    while (1)
    {
		taskENTER_CRITICAL();
        if (Serial_GetRxFlag() == 1) // 检查串口接收数据的标志位
        {
            RxData = Serial_GetRxData(); // 获取串口接收的数据
        }
		if(RxData == 'B') Motor_SetSpeed(0);
		if(RxData == 'A') Motor_SetSpeed(20);

		//printf("-- App_Task_Receive run \r\n");
		taskEXIT_CRITICAL();
        vTaskDelay(500);

    }
}

// 输出当前温度的函数
void App_Task_Display(void *args)
{
    while (1)
    {
		taskENTER_CRITICAL();
        display_wd();
		//printf("-- App_Task_Display run \r\n");
		OLED_ShowNum(6,1,RxData,4);
		taskEXIT_CRITICAL();
		vTaskDelay(500);
    }
}



