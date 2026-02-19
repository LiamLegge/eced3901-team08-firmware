#include "app.h"
#include "main.h"
#include "ARGB.h"
#include "stm32_ros_topic.h"
#include "sr04.h"
#include "stm32g0xx_hal.h"
// #include "led.h"
#include "fsk.h"
// #include "sensor.h"
// #include "cargo.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"
#include "logging.h"

// Handles
extern UART_HandleTypeDef huart2;

__weak void init_led(void)    {}
__weak void led_main(void)    {}

__weak void init_sensor(void) {}
__weak void sensor_main(void) {}

__weak void init_sr04(void) {}
__weak void sr04_main(void) {}

__weak void init_cargo(void)  {}
__weak void cargo_main(void)  {}


// Subsystem startup functions
void app_init(void)
{
    init_logging(&huart2);
    init_led();
    init_fsk((uint8_t*)"BYE");
    init_sensor();
    init_sr04();
    init_cargo();
}


// Application main loop    
void app(void)
{
    print_log("[ LOG ] === SYSTEM START ===");
    app_init();
 
    for (;;)
    {
        profile_begin();

        ros_topic_main();
        led_main();
        fsk_main(0x01);
        sr04_main();
        cargo_main();
        
        profile_end();
        HAL_Delay(1000);
    }
}
