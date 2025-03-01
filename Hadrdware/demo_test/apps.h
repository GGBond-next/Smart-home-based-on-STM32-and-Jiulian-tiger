#ifndef __APPS_H
#define __APPS_H

static const uint8_t g_scoreNotes[] = {
    1, 2, 3, 1,        1, 2, 3, 1,        3, 4, 5,  3, 4, 5,
    5, 6, 5, 4, 3, 1,  5, 6, 5, 4, 3, 1,  1, 8, 1,  1, 8, 1
};

#define SONG_LENGTH (sizeof(g_scoreNotes) / sizeof(uint8_t))
void pwm_init(void);
void pwm_set(void);
void touch_init(void);
int touch_set(void);
int adc_get(void);
int usart_init(void);
int usart_read(void);
void usart_submit(void);
void gpio_init(void);
void music_init(void);
void play_song(int i);

#endif
