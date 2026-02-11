#ifndef APP_H
#define APP_H

#include "main.h"
#include "led.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"

void app(void);
void delay(uint16_t time);
void HCSR04_Read(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif
