#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hilog/log.h"
#include "um_pwm.h"
#include "um_gpio.h"
#include "um_adc.h"
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "serial_uart.h"
#include <termios.h> // 添加此行以包含 tcflush 和相关宏

#define INIT_DUTY 500000
#define MAX_DUTY 2500000


int pwmChannel = PWM1;
int isEnable = PWM_IS_ENABLED;
int cur_duty = 0;
int pwm_duty = 0;
int temp_duty = 0;
int final_duty = 0;



int getValue = -1;
int adcValue = 0;
uint8_t ans = 0;
static int fd;
int touch_Num = UM_GPIO_06;
int gpioNum = UM_GPIO_07;
int bExport = UM_GPIO_EXPORTED;
int led_direction = UM_GPIO_DIRECTION_OUT;
int touch_direction = UM_GPIO_DIRECTION_IN;
int value = UM_GPIO_LOW_LEVE;


int get_final_duty(int pwm_duty, int final_duty)
{
    int real_duty = 0;
    // 判断最终转到的角度是否超过溢出值，输出实际转到的角度
    if (final_duty < INIT_DUTY) {
        real_duty = INIT_DUTY;
        printf("Beyond the angle of rotation");
    } else if (final_duty > MAX_DUTY) {
        real_duty = MAX_DUTY;
        printf("Beyond the angle of rotation");
    } else {
        real_duty = final_duty;
    }
    return real_duty;
}


void pwm_init(void)
{
    init_pmw(pwmChannel);
    set_pwm_period(pwmChannel, 2600000L);
    set_pwm_dutyCycle(pwmChannel, temp_duty);
    cur_duty = get_pwm_dutyCycle(pwmChannel);
    if (cur_duty < INIT_DUTY) {
        set_pwm_dutyCycle(pwmChannel, INIT_DUTY);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
    }
}

void pwm_set(void)
{
    
    if( adcValue <= 1000 )
    {
        temp_duty = 2500000;
        final_duty = temp_duty + cur_duty;
        final_duty = get_final_duty(temp_duty, final_duty);
        set_pwm_dutyCycle(pwmChannel, final_duty);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
        // 计算旋转后的角度
        printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
        set_pwm_enable(pwmChannel, isEnable);
        printf(" ------ temp_duty1 \n");
    }
    else if(adcValue >= 3000)
    {
        temp_duty = -2500000;
        final_duty = temp_duty + cur_duty;
        final_duty = get_final_duty(temp_duty, final_duty);
        set_pwm_dutyCycle(pwmChannel, final_duty);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
        // 计算旋转后的角度
        printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
        set_pwm_enable(pwmChannel, isEnable);
        printf(" ------ temp_duty2 \n");
    }
}

void touch_init(void)
{
    UM_GPIO_Export(touch_Num, bExport);
    UM_GPIO_Export(gpioNum, bExport);
    UM_GPIO_SetDirection(touch_Num, touch_direction);
    UM_GPIO_SetDirection(gpioNum, led_direction);
    UM_GPIO_SetValue(touch_Num, value);
    UM_GPIO_SetValue(gpioNum, value);
    
}

void touch_set(void)
{
    UM_GPIO_GetValue(touch_Num,&getValue);
    if(getValue == 1)
    {
        UM_GPIO_SetValue(gpioNum,1);
    }else{
        UM_GPIO_SetValue(gpioNum,0);
    }
    printf("----- -- touch_value == %d" , getValue);
}

int adc_get(void)
{
    // 获取ADC数据
    if (get_adc_data(ADC_1, &adcValue) == ADC_OK) {
        //if(adcValue <= 1000)
         //printf("too  light -- close\n");
        //if(adcValue >= 3000)
         //printf("too dark -- open\n");
         return adcValue;
    } 
    //else {
        //printf("Failed to get ADC data\n");
    //}
    return -1;
    
}

int usart_init(void)
{
    char *uart_dev = UART_TTL_NAME;
    int ret = ERR;

    fd = open(uart_dev, O_RDWR);
    if (fd == ERR) {
        perror("open file fail\n");
        return ERR;
    }
    ret = uart_init(fd, 9600L);
    if (ret == ERR) {
        perror("uart init error\n");
        return ERR;
    }
    return 3;
}

uint8_t usart_read(void)
{
    int ret = 0;
    ret = read(fd, &ans, 1);
    return ans;
}

//不用一直发送和接受
void usart_submit(void)
{
    int ret = 0;
    if(ans == 61)
    {
        const char *data_to_send = "A";
        size_t data_length = strlen(data_to_send);
        ret = send_data(fd, data_to_send, data_length);
        if (ret == ERR) {
            perror("send data error\n");
        }
        ans = 0;
    }
    if(ans == 62)
    {
        const char *data_to_send = "B";
        size_t data_length = strlen(data_to_send);
        ret = send_data(fd, data_to_send, data_length);
        if (ret == ERR) {
            perror("send data error\n");
        }
        ans = 0;
    }
}


void usart_send(int value)
{
    send_data(fd, &value, 1);
}