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
#include "iot_pwm.h"
#include "iot_gpio.h"   // OpenHarmony API：IoT硬件设备操作接口-GPIO
#include "hi_io.h"      // 海思 Pegasus SDK：IoT硬件设备操作接口-IO
#include "hi_pwm.h"     // 海思 Pegasus SDK：IoT硬件设备操作接口-PWM

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

int LEDpwm = UM_GPIO_01;
int LEDadc = UM_GPIO_02;
int LEDusartr = UM_GPIO_03;
int LEDusarts = UM_GPIO_04;

int bExport = UM_GPIO_EXPORTED;
int led_direction = UM_GPIO_DIRECTION_OUT;
int touch_direction = UM_GPIO_DIRECTION_IN;
int value = UM_GPIO_LOW_LEVE;
int hightvalue = UM_GPIO_HIGH_LEVE;



void gpio_init(void)
{
    UM_GPIO_Export(LEDpwm, bExport);
    UM_GPIO_Export(LEDadc, bExport);
    UM_GPIO_Export(LEDusartr, bExport);
    UM_GPIO_Export(LEDusarts, bExport);


    UM_GPIO_SetDirection(LEDpwm, led_direction);
    UM_GPIO_SetDirection(LEDadc, led_direction);
    UM_GPIO_SetDirection(LEDusartr, led_direction);
    UM_GPIO_SetDirection(LEDusarts, led_direction);


    UM_GPIO_SetValue(LEDusartr, value);
    UM_GPIO_SetValue(LEDusarts, value);
    UM_GPIO_SetValue(LEDpwm, value);
    UM_GPIO_SetValue(LEDadc, value);
}

/*
PWM功能

通过 adcValue 控制PWM信号，用于调节硬件输出（如舵机角度或LED亮度）。
支持占空比限制，防止硬件运行超出范围。
触摸输入

检测触摸传感器状态，通过LED反映触摸事件。
模拟开关的行为，适合简单的输入控制场景。
ADC模块

获取传感器的光强值，动态调整PWM输出，增强系统自动化能力。
串口通信

接收外部命令（通过串口），执行相应操作并反馈状态。
提供与外部设备的交互能力。
*/



/*
检查最终占空比是否超出范围。
超出范围时限制到初始值或最大值。
*/
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

/*
初始化PWM模块。
设置PWM周期为2600000L（单位通常是纳秒）。
如果当前占空比小于初始值 INIT_DUTY，则将其设置为初始值。
*/
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
/*
根据 adcValue 的值调整PWM信号。
如果 adcValue <= 1000，设置PWM为高占空比（temp_duty = 2500000）。
如果 adcValue >= 3000，设置PWM为低占空比（temp_duty = -2500000）。
调用 get_final_duty() 检查占空比是否超出范围（INIT_DUTY 到 MAX_DUTY），调整到有效值。
更新占空比后，计算角度变化并打印。
*/
void pwm_set(void)
{
    
    if( adcValue <= 1000 )
    {
        UM_GPIO_SetValue(LEDpwm,1);
        temp_duty = 2500000;
        final_duty = temp_duty + cur_duty;
        final_duty = get_final_duty(temp_duty, final_duty);
        set_pwm_dutyCycle(pwmChannel, final_duty);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
        // 计算旋转后的角度
        printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
        set_pwm_enable(pwmChannel, isEnable);
        printf(" ------ temp_duty1 \n");
        UM_GPIO_SetValue(LEDpwm,0);
    }
    else if(adcValue >= 3000)
    {
        UM_GPIO_SetValue(LEDpwm,1);
        temp_duty = -2500000;
        final_duty = temp_duty + cur_duty;
        final_duty = get_final_duty(temp_duty, final_duty);
        set_pwm_dutyCycle(pwmChannel, final_duty);
        cur_duty = get_pwm_dutyCycle(pwmChannel);
        // 计算旋转后的角度
        printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
        set_pwm_enable(pwmChannel, isEnable);
        printf(" ------ temp_duty2 \n");
        UM_GPIO_SetValue(LEDpwm,0);
    }
}
/*
初始化触摸输入引脚和LED控制引脚。
将触摸引脚设置为输入模式，LED引脚设置为输出模式。
初始化为低电平。
*/
void touch_init(void)
{
    UM_GPIO_Export(touch_Num, bExport);
    UM_GPIO_Export(gpioNum, bExport);
    UM_GPIO_SetDirection(touch_Num, touch_direction);
    UM_GPIO_SetDirection(gpioNum, led_direction);
    UM_GPIO_SetValue(touch_Num, value);
    UM_GPIO_SetValue(gpioNum, hightvalue);
    
}
/*
检测触摸引脚的状态。
如果检测到高电平（触摸事件）。
如果检测到低电平执行相应的功能。
*/
int touch_set(void)
{
    UM_GPIO_GetValue(touch_Num,&getValue);
    if(getValue == 1)
    {
        UM_GPIO_SetValue(gpioNum, 0);
    }else{
        UM_GPIO_SetValue(gpioNum, 1);
    }
    printf("----- -- touch_value == %d" , getValue);
    return 0;
}
/*
调用 get_adc_data() 获取ADC值。
如果获取成功，根据 adcValue 打印当前光强状态：
光强过高（adcValue <= 1000）：输出提示 "too light -- close"。
光强过低（adcValue >= 3000）：输出提示 "too dark -- open"。
*/
int adc_get(void)
{
    // 获取ADC数据
    if (get_adc_data(ADC_1, &adcValue) == ADC_OK) {
        if(adcValue <= 1000)
         {
            UM_GPIO_SetValue(LEDadc,1);
            printf("too  light -- close\n");
         }
        if(adcValue >= 3000)
         {
            UM_GPIO_SetValue(LEDadc,1);
            printf("too dark -- open\n");
         }
    } else {
        UM_GPIO_SetValue(LEDadc,0);
        printf("Failed to get ADC data\n");
    }
    return adcValue;
}
/*
初始化串口通信。
打开指定的串口设备文件（UART_TTL_NAME），并设置波特率为9600。
返回文件描述符，用于后续的读写操作。
*/
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
/*
从串口读取一个字节的数据，存储到 ans 变量中并返回。
*/
uint8_t usart_read(void)
{
    int ret = 0;
    ret = read(fd, &ans, 1);
    if(ans != 0)
    {
        UM_GPIO_SetValue(LEDusartr,1);
    }
    else
    {
        UM_GPIO_SetValue(LEDusartr,0);
    }
    
    return ans;
}

//不用一直发送和接受
/*
根据 ans 的值发送数据：
如果 ans == 61（ASCII码对应字符 =），发送字符 A。
如果 ans == 62（ASCII码对应字符 >），发送字符 B。
在每次发送数据后，将 ans 重置为0。
*/
void usart_submit(void)
{
    int ret = 0;
    if(ans == 61)
    {
        UM_GPIO_SetValue(LEDusarts,1);
        const char *data_to_send = "A";
        size_t data_length = strlen(data_to_send);
        ret = send_data(fd, data_to_send, data_length);
        if (ret == ERR) {
            perror("send data error\n");
        }
        ans = 0;
        UM_GPIO_SetValue(LEDusarts,0);
    }
    if(ans == 62)
    {
        UM_GPIO_SetValue(LEDusarts,1);
        const char *data_to_send = "B";
        size_t data_length = strlen(data_to_send);
        ret = send_data(fd, data_to_send, data_length);
        if (ret == ERR) {
            perror("send data error\n");
        }
        ans = 0;
        UM_GPIO_SetValue(LEDusarts,0);
    }
}

// 定义音符和对应的频率（单位：Hz），包括高八度和低八度
typedef struct {
    uint8_t note; // 简谱中的数字表示
    int frequency; // 频率
} Note;

// 音符频率表（假设中央C为261.63 Hz）
Note note_frequencies[] = {
    {0, 0},   // Rest (休止符)
    {1, 262}, // C
    {2, 294}, // D
    {3, 330}, // E
    {4, 349}, // F
    {5, 392}, // G
    {6, 440}, // A
    {7, 494}, // B
    {8, 523}  // High C
};

#define SONG_LENGTH (sizeof(g_scoreNotes) / sizeof(uint8_t))

static const uint8_t g_scoreNotes[] = {
    1, 2, 3, 1,        1, 2, 3, 1,        3, 4, 5,  3, 4, 5,
    5, 6, 5, 4, 3, 1,  5, 6, 5, 4, 3, 1,  1, 8, 1,  1, 8, 1
};

// 设置PWM周期以匹配频率
void set_pwm_frequency(int pwmChannel, int frequency) {
    if (frequency == 0) {
        // 如果频率为0，则关闭PWM
        set_pwm_enable(pwmChannel, PWM_NOT_ENABLED);
        return;
    }

    // 假设系统时钟周期为1纳秒 (1e9 ns)
    // 计算周期（period）和高电平时间（duty cycle），使占空比为50%
    int period = 1e9 / frequency;
    int dutyCycle = period / 2;

    set_pwm_period(pwmChannel, period);
    set_pwm_dutyCycle(pwmChannel, dutyCycle);
    set_pwm_enable(pwmChannel, PWM_IS_ENABLED);
}

// 播放一首歌
void play_song(int i) {
    // for (int i = 0; i < SONG_LENGTH; ++i) {
        uint8_t current_note = g_scoreNotes[i];
        Note note_info = note_frequencies[current_note];

        printf("Playing note: %d\n", current_note);

        // 设置频率
        set_pwm_frequency(2, note_info.frequency);

        // 等待一段时间模拟音符长度（这里假设每个音符持续0.5秒）
        usleep(500000); // 0.5秒

        // 短暂停顿以区分音符
        set_pwm_frequency(pwmChannel, 0); // 关闭PWM以创建间隙
        usleep(100000); // 0.1秒的停顿
    //}
}

void music_init(void) {
    int pwmChannel = PWM2; // 选择PWM通道
    int result;

    // 初始化PWM
    result = init_pmw(pwmChannel);
    if (result != 0) {
        printf("Failed to initialize PWM channel %d\n", pwmChannel);
        return ;
    }

    // 设置极性为正常
    result = set_pwm_polarity(pwmChannel, PWM_POLARITY_NORMAL);
    if (result != 0) {
        printf("Failed to set PWM polarity for channel %d\n", pwmChannel);
        return;
    }
}

