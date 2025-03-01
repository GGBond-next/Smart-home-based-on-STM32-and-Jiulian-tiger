#ifndef __APPS_H
#define __APPS_H
#ifdef __cplusplus
extern "C" {
#endif

#define INIT_DUTY 500000
#define MAX_DUTY 2500000
void pwm_init(void);
void pwm_set(void);
void touch_init(void);
void touch_set(void);
int adc_get(void);
int usart_init(void);
int usart_read(void);
void usart_submit(void);
void usart_send(char value);
#ifdef __cplusplus
}
#endif

#endif
