#include "timer.h"
#include "sr04.h"
#include "stm32g0xx_hal.h"

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
    
    if(htim->Instance == TIM3){
        SR04_TIM_IC_Callback(htim);
    }
}
