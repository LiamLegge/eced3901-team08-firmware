#include "led.h"
#include "sr04.h"
#include "ARGB.h"   
#include "math.h"
#include "stm32g0xx_hal.h"
#include "logging.h"
#include <stdbool.h>

// Extern Defines
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim1_ch2;

// Global Defines
#define VERBOSE false

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
    ARGB_SetRGB(1, 128, 0,   0);     // Red
    ARGB_SetRGB(2, 128, 128, 128);   // White
    ARGB_SetRGB(3, 0,   128, 0);     // Green
    ARGB_SetRGB(4, 128, 128, 128);   // White
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
    ARGB_SetRGB(0, 0, 255, 0);
}
// Display med danger on the distance LED
void show_dangermed(void){

    led_default();
    ARGB_SetRGB(0, 255, 255, 0);
}

// Display high danger on the distance LED
void show_dangerhig(void) {
    led_default();
    ARGB_SetRGB(0, 255, 0, 0);
}

// Display oscillating on distance LED
void show_collected(void) {
    uint16_t t = (uint16_t)(HAL_GetTick()/10);
    led_default();

    uint8_t hue = 35; // Gold
    uint8_t sat = 255;
    uint8_t val = oscillateBrightness(t, 60.00f, 0, 255);
    ARGB_SetHSV(0, hue, sat, val);
}


// LED Init
void init_led(void) {
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn); // Enable the interrupt for the timer
    ARGB_Init();  // Initialization
    ARGB_SetBrightness(5); // Set a moderate global brightness (0-255)
    ARGB_Clear();
    ARGB_Show();
}

t_ShowType check_show(uint16_t minDistance, uint16_t distance1) {
    #define AVG_WINDOW_SIZE 5
    static uint16_t minDistanceBuffer[AVG_WINDOW_SIZE] = {0};
    static uint16_t distance1Buffer[AVG_WINDOW_SIZE] = {0};
    static int bufferIndex = 0;
    static int bufferCount = 0;

    // Update buffers
    minDistanceBuffer[bufferIndex] = minDistance;
    distance1Buffer[bufferIndex] = distance1;
    bufferIndex = (bufferIndex + 1) % AVG_WINDOW_SIZE;
    if (bufferCount < AVG_WINDOW_SIZE) bufferCount++;

    // Calculate averages
    uint32_t minDistanceSum = 0;
    uint32_t distance1Sum = 0;
    for (int i = 0; i < bufferCount; ++i) {
        minDistanceSum += minDistanceBuffer[i];
        distance1Sum += distance1Buffer[i];
    }
    uint16_t minDistanceAvg = (bufferCount > 0) ? (uint16_t)(minDistanceSum / bufferCount) : 0;
    uint16_t distance1Avg = (bufferCount > 0) ? (uint16_t)(distance1Sum / bufferCount) : 0;

    t_ShowType candidateShow = SHOW_OFF;
    if(distance1Avg > 0 && distance1Avg <= 5){
        candidateShow = SHOW_COLLECTED;
    }
    else if(minDistanceAvg > 0 && minDistanceAvg <= 62){
        candidateShow = SHOW_DANGERHIG;
    }
    else if(minDistanceAvg > 62 && minDistanceAvg <= 122){
        candidateShow = SHOW_DANGERMED;
    }
    else{
        candidateShow = SHOW_DANGERLOW;
    }

    currentShow = candidateShow;
    return currentShow;
}

// Switch case to set the light mode
void led_main(void){

    uint16_t distance1 = sr04_read(0);
    uint16_t distance2 = sr04_read(1);

    // Simple Sorter
    uint16_t minDistance = (distance1 < distance2) ? distance1 : distance2;
    
    t_ShowType currentShow = check_show(minDistance, distance1); 

    if (VERBOSE) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[ LED ] Dist1: (cm):   %lu", (unsigned long)distance1);
        print_log(buf);
        snprintf(buf, sizeof(buf), "[ LED ] Dist2: (cm):   %lu", (unsigned long)distance2);
        print_log(buf);
        snprintf(buf, sizeof(buf), "[ LED ] Show:           %d", (int)currentShow);
        print_log(buf);
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
            show_collected();
            break;
        default:
            break;
    }
    ARGB_Show();
}
