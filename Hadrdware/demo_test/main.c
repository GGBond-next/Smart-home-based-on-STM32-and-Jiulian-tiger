
// #include <string.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>
// #include "ohos_init.h"
// #include "cmsis_os2.h"
// #include "hilog/log.h"
// #include "um_pwm.h"
// #include "um_gpio.h"
// #include "um_adc.h"
// #include <math.h>
// #include <sys/types.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <pthread.h>
// #include "serial_uart.h"

// #define INIT_DUTY 500000
// #define MAX_DUTY 2500000

// int main(int argc, char **argv)
// {
//     int pwmChannel = PWM1;
//     int isEnable = PWM_IS_ENABLED;
//     // int final_duty = 0;
//     //double cur_angle = 0;
//     int cur_duty = 0;
//     int pwm_pin = atoi(argv[1]);
//     int pwm_duty = 0;
//     int temp_duty =0;


//     int touch_Num = UM_GPIO_06;
//     int gpioNum = UM_GPIO_07;
//     int bExport = UM_GPIO_EXPORTED;
//     int led_direction = UM_GPIO_DIRECTION_OUT;
//     int touch_direction = UM_GPIO_DIRECTION_IN;
//     int value = UM_GPIO_LOW_LEVE;
//     int getValue = -1;
//     // int getValue_led = -1;
//     // int getValue_touch = -1;

//     UM_GPIO_Export(touch_Num, bExport);
//     UM_GPIO_Export(gpioNum, bExport);
//     UM_GPIO_SetDirection(touch_Num, touch_direction);
//     UM_GPIO_SetDirection(gpioNum, led_direction);
//     UM_GPIO_SetValue(touch_Num, value);
//     UM_GPIO_SetValue(gpioNum, value);


//     if (argc > 3L || argc < 2L) {
//         user_help();
//         HILOG_DEBUG(LOG_CORE, "The number of parameters is incorrect!\n");
//         return PWM_ERR;
//     }

//     init_pmw(pwmChannel);
//     set_pwm_period(pwmChannel, 2600000L);

//     set_pwm_dutyCycle(pwmChannel, temp_duty);
//     while(1)
//     {
//     int adcValue = 0;

//     // 获取ADC数据
//     if (get_adc_data(ADC_1, &adcValue) == ADC_OK) {
//         if(adcValue == 0)
//          printf("too  light -- close\n");
//         if(adcValue == 4095)
//          printf("too dark -- open\n");
//     } else {
//         printf("Failed to get ADC data\n");
//     }

//         UM_GPIO_GetValue(touch_Num,&getValue);

//         if( adcValue != 0 && temp_duty == -2500000)
//         {
//             temp_duty = 2500000;
//             set_pwm_dutyCycle(pwmChannel, temp_duty);
//             cur_duty = get_pwm_dutyCycle(pwmChannel);
//             // 计算旋转后的角度
//             printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
//             set_pwm_enable(pwmChannel, isEnable);
//         }
//         if(adcValue == 4095 && temp_duty == 2500000)
//         {
//             temp_duty = -2500000;
//             set_pwm_dutyCycle(pwmChannel, temp_duty);
//             cur_duty = get_pwm_dutyCycle(pwmChannel);
//             // 计算旋转后的角度
//             printf("The angle after rotation is %.2f.\n", 1.0 * (cur_duty - INIT_DUTY) / INIT_DUTY * 45L);
//             set_pwm_enable(pwmChannel, isEnable);
//         }

//         if(getValue == 1)
//         {
//             UM_GPIO_SetValue(gpioNum,1);
//         }else{
//             UM_GPIO_SetValue(gpioNum,0);
//         }





//         usleep(600000);
//     }
//     return 0;
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <pthread.h>
// #include "serial_uart.h"

// static int fd;

// // 从串口读的线程
// void *_serial_input_task(void)
// {
//     //int i = 0;
//     int ret = ERR;
//     int buf = 0;
//     //int recv[FRAME_LEN] = {0};

//     //printf("Gesture Sensor Ready!\n");
//     // while (1) {
//         ret = read(fd, &buf, 1);
//         printf("BUF == %d" , (uint8_t)buf);
        // for (i = 0; i < FRAME_LEN; i++) {
        //     ret = read(fd, &buf, 1);
        //     if (ret == ERR) {
        //         perror("read error\n");
        //         exit(0);
        //     }
        //     recv[i] = buf;
        // }
        // ret = data_proce(recv);
        // if (ret == ERR) {
        //     perror("error\n");
        //     exit(0);
        // }
    // }
//     exit(0);
// }

// int main(int argc, char **argv)
// {
//     char *uart_dev = UART_TTL_NAME;
//     int ret = ERR;

//     fd = open(uart_dev, O_RDWR);
//     if (fd == ERR) {
//         perror("open file fail\n");
//         return ERR;
//     }
//     ret = uart_init(fd, 9600L);
//     if (ret == ERR) {
//         perror("uart init error\n");
//         return ERR;
//     }

//     // 创建线程  一直执行读串口的操作
//     pthread_t pid_t;
//     pthread_create(&pid_t, NULL, (void *)_serial_input_task, 0);
//     const char *data_to_send = "ABCD";
//     size_t data_length = strlen(data_to_send);

//     ret = send_data(fd, data_to_send, data_length);
//     if (ret == ERR) {
//         perror("send data error\n");
//     }

//     while (1) {
//         sleep(10L);
//     }
//     close(fd);

//     return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "serial_uart.h"
#include "apps.h"
#include "um_pwm.h"

static void *pwm(const char *arg)
{
    (void)arg;
    while (1) {
        pwm_set();
        printf("---- pwm_set runing ----- \n");
         usleep(500000);;

    }

    return NULL;
}


static void *adc(const char *arg)
{
    (void)arg;
    while (1) {
        printf("adc == %d\n",adc_get());
        usleep(500000);;

    }

    return NULL;
}


static void *usart_s(const char *arg)
{
    (void)arg;
    while (1) {
        usart_submit();
        printf("---- usart_s runing ----- \n");

        usleep(500000);
    }

    return NULL;
}


static void *usart_r(const char *arg)
{
    (void)arg;
    while (1) {
        uint8_t ans = usart_read();
        
        printf("---- usart_r = %d ----- \n",ans);
        usleep(500000);;

    }
    return NULL;
}

int tt = 0;
static void *touch(const char *arg)
{
    (void)arg;
    while (1) {
        tt = touch_set();
        printf("---- touch_set runing ----- \n");
        usleep(500000);;
    }
    return NULL;
}

// static void *music(const char *arg)
// {
//     (void)arg;
//     size_t i = 0;
//     while (1) {
//         if(tt == 1)
//         {play_song(i);
//         if(i < SONG_LENGTH) i++;
//         else i = 0;
//         printf("---- music runing ----- \n");}
//         else {
//                 set_pwm_dutyCycle(2, 0);
//                 set_pwm_enable(2, PWM_NOT_ENABLED);}
//         usleep(200000);
//     }
    
//     return NULL;
// }


int main(int argc, char **argv) {
    gpio_init();
    pwm_init();
    touch_init();
    usart_init();
    music_init();


    pthread_t pid_t1;
    pthread_create(&pid_t1, NULL, (void *)pwm, 0);

    pthread_t pid_t2;
    pthread_create(&pid_t2, NULL, (void *)adc, 0);

    pthread_t pid_t3;
    pthread_create(&pid_t3, NULL, (void *)usart_s, 0);

    pthread_t pid_t4;
    pthread_create(&pid_t4, NULL, (void *)usart_r, 0);

    pthread_t pid_t5;
    pthread_create(&pid_t5, NULL, (void *)touch, 0);

    // pthread_t pid_t6;
    // pthread_create(&pid_t6, NULL, (void *)music, 0);
    
    // 对于非RTOS环境，这里可以是一个无限循环以保持程序运行。
    while (1) {
        sleep(1); // 防止CPU占用过高
    }

    return 0;
}