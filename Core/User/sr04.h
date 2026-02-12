#pragma once 

void delay(uint16_t time);

void SR04_Init(void);
void SR04_Read(void);
uint16_t SR04_GetDistance(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);