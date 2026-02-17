#include "led.h"
#include "sr04.h"
#include "ARGB.h"   

#include "math.h"
#include "stm32g0xx_hal.h"


// Extern Defines
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch1;

// Global Defines
#define FRAME_DELAY_MS 10 

uint32_t frame = 0;

// Commands Define
typedef enum {
    SHOW_OFF = 0,
    SHOW_DANGERLOW,
    SHOW_DANGERMED,
    SHOW_DANGERHIG,
    SHOW_COLLECTED,
    NUM_OF_SHOWS
} t_ShowType;
t_ShowType currentShow = 1;

// Utility function to make a simple "oscillating" brightness.
static uint8_t oscillateBrightness(float t, float period, uint8_t minVal, uint8_t maxVal) {
    // Create a normalized sine wave between minVal and maxVal
    float angle = (2.0f * M_PI * t) / period;
    float sineVal = (sin(angle) + 1.0f) / 2.0f; // map sine(-1..1) to (0..1)
    float range = (float)(maxVal - minVal);
    return (uint8_t)(minVal + sineVal * range);
}
// Set the default LED colors around the robot
void led_default(void){
    ARGB_SetHSV(0,0,255,128); //RED
    ARGB_SetHSV(1,0,0,128);   //WHITE
    ARGB_SetHSV(2,85,255,128);//GREEN
    ARGB_SetHSV(3,0,0,128);   //WHITE
}
// Turn off all the LEDs for startup
void show_off (void){
    ARGB_FillRGB(0,0,0);
    ARGB_FillHSV(0,0,0);
    ARGB_FillWhite(0);
}
// Display low danger on the distance LED
void show_dangerlow(void){

    led_default();

    // DISTANCE

    uint8_t hue = 43; // Yellow
    uint8_t sat = 255;
    uint8_t val = 128;
    ARGB_SetHSV(4, hue, sat, val);

}
// Display med danger on the distance LED
void show_dangermed(void){

    led_default();

    // DISTANCE

    uint8_t hue = 22; // Amber
    uint8_t sat = 255;
    uint8_t val = 128;
    ARGB_SetHSV(4, hue, sat, val);

}
// Display high danger on the distance LED
void show_dangerhig(void) {
    
    led_default();

    // DISTANCE

    uint8_t hue = 5; // Red
    uint8_t sat = 255;
    uint8_t val = 128;
    ARGB_SetHSV(4, hue, sat, val);
}
// Display oscillating on distance LED
void show_collected(uint32_t frame) {
    uint16_t t = (float)frame;

    led_default();

    // DISTANCE

    uint8_t hue = 35; // Gold
    uint8_t sat = 255;
    uint8_t val = oscillateBrightness(t, 60.00f, 0, 255);
    ARGB_SetHSV(4, hue, sat, val);
}


// LED Init
void init_led(void) {
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); // Enable the interrupt for the timer
    ARGB_Init();  // Initialization
    ARGB_SetBrightness(128); // Set a moderate global brightness (0-255)
    ARGB_Clear();
    ARGB_Show();
}
// Switch case to set the light mode
void led_main(void){


    sr04_read(0);
    HAL_Delay(60);
    uint16_t distance = get_distance();

    HAL_Delay(400);

    sr04_read(1);
    HAL_Delay(60);
    uint16_t distance1 = get_distance();

    if(distance1 < distance){
        distance = distance1;
    }

    HAL_Delay(200);

    for(;;) {
        while (ARGB_Ready() != ARGB_READY) { }
        ARGB_Show();
        // show_rainbow_fade(frame);
        ARGB_SetHSV(0,0,255,128);
        HAL_Delay(FRAME_DELAY_MS);
        frame++;
    }
    /*
    if(distance > 0 && distance <= 5){
        currentShow = 4;
    }
    else if(distance > 5 && distance <= 61){
        currentShow = 3;
    }
    else if(distance > 61 && distance <= 122){
        currentShow = 2;
    }
    else{
        currentShow = 1;
    }

    // Wait for strip to be ready
    while (ARGB_Ready() != ARGB_READY) { }
    switch (currentShow) {
        case SHOW_OFF:
            show_off();
            break;
        case SHOW_DANGERLOW:
            show_dangerlow();
            break;
        case SHOW_DANGERMED:
            show_dangermed();
            break;
        case SHOW_DANGERHIG:
            show_dangerhig();
            break;
        case SHOW_COLLECTED:
            show_collected(frame);
            break;
        default:
            break;
    }
    
    ARGB_Show();
    HAL_Delay(FRAME_DELAY_MS);
    frame++;
    */
}
