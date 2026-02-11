#include "app.h"
#include "ARGB.h"
#include "stm32g0xx_hal.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;

#define FRAMEDELAY 10

void show_rainbow_fade(uint32_t frame) {
    // The HSV hue is typically 0-255 for a full cycle of colors.
    // Let's pick a base hue that shifts over time, causing the rainbow to move.
    uint8_t baseHue = (uint8_t)(frame % 256); 

    // Iterate over each LED and assign a hue offset based on its index.
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
        // Spread the rainbow evenly across all LEDs.
        // For a continuous rainbow, let's map the LED index so that 
        // the entire strip covers 256 hue steps. 
        // If NUM_LEDS is small, this may repeat colors, but it still creates a gradient.
        uint8_t hueOffset = (uint8_t)((256UL * i) / NUM_PIXELS);
        uint8_t hue = (uint8_t)(baseHue + hueOffset);
        uint8_t saturation = 255;
        uint8_t value = 255;

        ARGB_SetHSV(i, hue, saturation, value);
    }
}

void app(void) {

    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

    ARGB_Init();
    ARGB_SetBrightness(128);
    ARGB_Clear();
    ARGB_Show();

    uint32_t frame = 0;

    // main program loop
    for(;;) {
        while (ARGB_Ready() != ARGB_READY) { }
        ARGB_Show();
        // show_rainbow_fade(frame);
        ARGB_SetHSV(0,0,255,128);
        HAL_Delay(FRAMEDELAY);
        frame++;
    }
}
