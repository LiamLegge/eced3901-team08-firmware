#pragma once 

#include "main.h"

uint16_t sr04_read(uint16_t c);
uint16_t get_distance(void);
void SR04_TIM_IC_Callback(TIM_HandleTypeDef *htim);

void init_sr04(void);
void sr04_main(void);
