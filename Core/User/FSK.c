#include "FSK.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include <string.h>

#define freq_1 (1000-1)
#define freq_0 (333-1)

void init_fsk(uint8_t* msg, uint32_t* pwm_data){
    for(int j=0; j < 3; j++){
        for(int i=0; i < 8; i++){
            if((msg[j] >> i) & 0x01){
                pwm_data[i] = freq_1;
            }else{
                pwm_data[i] = freq_0;
            }
        }
    }
}

void start_fsk(uint32_t* pwm_data){
    __HAL_TIM_SET_AUTORELOAD(&htim2, pwm_data[0]);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COUNTER(&htim15, 0);
    HAL_TIM_Base_Start_IT(&htim15);
    HAL_TIM_Base_Start(&htim2);
}

void stop_fsk(void){
    HAL_TIM_Base_Stop_IT(&htim15);
    HAL_TIM_Base_Stop(&htim2);
}
