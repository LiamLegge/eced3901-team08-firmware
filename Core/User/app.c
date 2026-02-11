#include "app.h"
#include "stm32g0b1xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_tim.h"

extern TIM_HandleTypeDef htim1;

volatile uint16_t Distance = 0;
volatile uint16_t Is_First_Captured = 0;
volatile uint16_t IC_Val1 = 0;
volatile uint16_t IC_Val2 = 0;
volatile uint16_t Difference = 0;

void app(void) {
    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);

    while(1){
        HCSR04_Read();
        HAL_Delay(200);
        if(Distance > 40){
            HAL_GPIO_TogglePin(GPIOA,  GPIO_PIN_7);
        }
    }
}

void delay(uint16_t time){
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while(__HAL_TIM_GET_COUNTER(&htim1) < time);
}

void HCSR04_Read(void){
    /* Drive the TRIG pin (use TRIG_Pin/TRIG_GPIO_Port defined in main.h) */
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  // if the interrupt source is channel1
    {
        if (Is_First_Captured==0) // if the first value is not captured
        {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read the first value
            Is_First_Captured = 1;  // set the first captured as true
            // Now change the polarity to falling edge
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (Is_First_Captured==1)   // if the first is already captured
        {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  // read second value
            __HAL_TIM_SET_COUNTER(htim, 0);  // reset the counter

            if (IC_Val2 > IC_Val1)
            {
                Difference = IC_Val2 - IC_Val1;
            }
            else if (IC_Val1 > IC_Val2)
            {
                Difference = (0xFFFF - IC_Val1) + IC_Val2;
            }

            /* calculate distance (cm): Difference * 0.034 / 2 */
            Distance = Difference * 0.034/2.0;
            Is_First_Captured = 0; // set it back to false

            // set polarity to rising edge
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
            __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
        }
    }
}

