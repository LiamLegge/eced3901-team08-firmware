#include "app.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;

#define FRAMEDELAY 10

void app(void) {
    led_main();
}
