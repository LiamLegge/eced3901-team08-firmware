#pragma once 

#include "main.h"
#include "stm32g0xx_hal_tim.h"

void delay(uint16_t time);

void init_sr04(void);
uint16_t sr04_read(void);
void SR04_TIM_IC_Callback(TIM_HandleTypeDef *htim);
