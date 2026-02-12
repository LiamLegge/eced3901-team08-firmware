#pragma once 

void delay(uint16_t time);

void init_sr04(void);
uint16_t sr04_read(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
