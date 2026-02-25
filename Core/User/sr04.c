#include "sr04.h"
#include "timer.h"
#include "stm32g0xx_hal_tim.h"
#include "stdbool.h"

#define MAX_TIMEOUT_MS 100

// Extern Defines
extern TIM_HandleTypeDef htim3;

// Global Defines
static volatile uint16_t Distance = 0;
static volatile uint8_t  Is_First_Captured = 0;
static volatile uint16_t IC_Val1 = 0;
static volatile uint16_t IC_Val2 = 0;
static volatile uint16_t Difference = 0;
static volatile bool dataReady = false;

// Internal delay (us)
static void delay_us(uint16_t time){
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < time);
}
// Read Function
uint16_t sr04_read(uint16_t c){
    if(c == 1){
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
        delay_us(10);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    }
    else{
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        delay_us(10);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    }
    __HAL_TIM_ENABLE_IT(&htim3, TIM_IT_CC1);
    return get_distance();
}
// Getter Function
uint16_t get_distance(void){
    uint32_t start = HAL_GetTick();

    while (!dataReady)
    {
        if ((HAL_GetTick() - start) >= MAX_TIMEOUT_MS) {
            dataReady = false;
            return 0xFFFF; // invalid / timeout
        }
    }

    dataReady = false; 
    return Distance;
}
// Interrupt
void SR04_TIM_IC_Callback(TIM_HandleTypeDef *htim){
    if (htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
        return;

    if (!Is_First_Captured)
    {
        IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        Is_First_Captured = 1;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
                                     TIM_INPUTCHANNELPOLARITY_FALLING);
    }
    else
    {
        IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        __HAL_TIM_SET_COUNTER(htim, 0);

        if (IC_Val2 > IC_Val1)
            Difference = IC_Val2 - IC_Val1;
        else
            Difference = (0xFFFF - IC_Val1) + IC_Val2;

        /* Distance in cm */
        Distance = (uint16_t)(Difference * 0.034f / 2.0f);
        
        Is_First_Captured = 0;
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1,
        TIM_INPUTCHANNELPOLARITY_RISING);
        __HAL_TIM_DISABLE_IT(&htim3, TIM_IT_CC1);
        dataReady = true;
    }
}

// Init Sensor
uint16_t init_sr04(void){
    HAL_TIM_Base_Start(&htim3);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
    return sr04_present();
}
// Timeout Disconnected
uint16_t sr04_present(void){
    uint16_t timeout = get_distance();
    for(uint16_t i = 0; i < 10; i++){
        if(timeout == 0xFFFF){
            return 1;
        }
    }
    return 0;
}
