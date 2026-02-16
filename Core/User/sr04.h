#pragma once 

#include "main.h"

void delay(uint16_t time);
uint16_t sr04_read(void);
void SR04_TIM_IC_Callback(TIM_HandleTypeDef *htim);

void init_sr04(void);
void sr04_main(void);
