#ifndef LED_H
#define LED_H

#include "math.h"
#include "ARGB.h"   
#include "stm32g0xx_hal.h"

/* Easy definition of LEDs
#define PORT 0
#define BOW 1
#define STARBOARD 2
#define PORT 3
#define DISTANCE 4
*/

typedef enum {
    SHOW_OFF = 0,
    SHOW_DANGERLOW,
    SHOW_DANGERMED,
    SHOW_DANGERHIG,
    SHOW_COLLECTED,
    NUM_OF_SHOWS
} t_ShowType;

t_ShowType currentShow = 1;

void led_main(void);
void led_set_show(t_ShowType show);

#endif