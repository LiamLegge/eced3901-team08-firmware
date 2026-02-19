#include "fsk.h"
#include "FSK.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include <string.h>

#define freq_1 (1000-1)
#define freq_0 (333-1)
#define dty_cyc1 (freq_1/2)
#define dty_cyc0 (freq_0/2)

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim15;
extern DMA_HandleTypeDef hdma_tim1_ch1;

uint32_t pwm_data[24];
int data_pos = 0;

void init_fsk(uint8_t* msg){
    int count = 0;
    for(int j=0; j < 3; j++){
        for(int i=0; i < 8; i++){
            if((msg[j] >> i) & 0x01){
                pwm_data[count] = freq_1;
                count++;
            } else{
                pwm_data[count] = freq_0;
                count++;
            }
        }
    }
}


void start_fsk(void){
    __HAL_TIM_SET_AUTORELOAD(&htim2, pwm_data[0]);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    __HAL_TIM_SET_COUNTER(&htim15, 0);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_Base_Start_IT(&htim15);
}

void stop_fsk(void){
    HAL_TIM_Base_Stop_IT(&htim15);
    HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_2);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM15){
        data_pos++;
        if(data_pos > 23){
            stop_fsk();
            data_pos = 0;
        }else{
            __HAL_TIM_SET_AUTORELOAD(&htim2, pwm_data[data_pos]);
            __HAL_TIM_SET_COUNTER(&htim2, 0);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_data[data_pos]/2);
        }
    }
}


int test_fsk(void) {
    // todo: create logic
    return 0;
}

// This function is the "main" loop of the FSK, 
int fsk_main(fskCommand_t cmd) {

    switch(cmd) {
        case FSK_CMD_IDLE:
            break;
        case FSK_CMD_START:
            break;
        case FSK_CMD_STOP:
            break;
        case FSK_CMD_TEST:
            break;
        default:
            break;
    }
    return 0;
}