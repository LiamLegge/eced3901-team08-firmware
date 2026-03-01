#include "app.h"
#include "main.h"
#include "ARGB.h"
#include "stm32_ros_topic.h"
#include "sr04.h"
#include "stm32g0xx_hal.h"
// #include "led.h"
#include "fsk.h"
// #include "sensor.h"
#include "cargo.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"
#include "logging.h"
#include "commands.h"
#include <stdint.h>

// Handles
extern UART_HandleTypeDef huart2;

__weak void init_led(void)    {}
__weak void led_main(void)    {}

__weak void init_sensor(void) {}
__weak void sensor_main(void) {}

__weak void init_sr04(void) {}
__weak void sr04_main(void) {}


// Subsystem startup functions
void app_init(void)
{
    init_logging(&huart2);
    init_commands();
    init_led();
    init_fsk((uint8_t*)"BYE");
    init_sensor();
    init_sr04();
    init_cargo();
}

void print_status_update(uint32_t time_ms) {
    const uint32_t log_interval_ms = 250;
    static uint32_t last_log_time = 0;

    if ((time_ms - last_log_time) >= log_interval_ms) {
        last_log_time = time_ms;

        // Print log message with a timestamp 
        print_log("-----------------------------");
        print_log("[ APP ] Time: %lu ms", time_ms);
        print_adc_value();
    }
}

// Application main loop    
void app(void)
{
    app_init();
    print_log("[ APP ] === SYSTEM START ===");

    char cmd = '0';
    uint32_t time_ms = 0;
    for (;;)
    {
        profile_begin();
        time_ms = HAL_GetTick();
        cmd = command_main();

        if (cmd == 0x01) {
            fsk_main(0x01);
            cmd = 0x00;
        }

        ros_topic_main();
        led_main();
        sr04_main();
        cargo_main(cmd);
        
        HAL_Delay(1);
        print_status_update(time_ms);
        profile_end();
    }
}
