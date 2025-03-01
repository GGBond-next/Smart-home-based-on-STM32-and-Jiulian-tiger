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
#include "app_task.h"







int main(void)
{
    // 模块初始化
    OLED_Init(); // OLED初始化
    Serial_Init(); // 串口初始化
    AD_Init();
	Motor_Init();
    //OLED_ShowString(1, 1, "getwd is");
	//printf("-- App_Task_Receive run\r\n");
    App_Task_Init();

    while (1)
    {
        // 主循环保持空转
		
    }
}
