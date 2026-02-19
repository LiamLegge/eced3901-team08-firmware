#include "app.h"
#include "ARGB.h"
#include "stm32g0b1xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include "FSK.h"
#include <stdint.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim15;
extern DMA_HandleTypeDef hdma_tim1_ch1;

#define FRAMEDELAY 10

void app(void) {
    init_fsk((uint8_t*)"BYE");
    start_fsk();

    // main program loop
    for(;;) {
        start_fsk();
        HAL_Delay(100);
    }
}
