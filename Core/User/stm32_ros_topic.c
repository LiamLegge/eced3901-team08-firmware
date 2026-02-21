#include "main.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"
#include "logging.h"



void ros_topic_main(void) {
    // Simple heartbeat
    
    static uint32_t last_time = 0;

    if (HAL_GetTick() - last_time >= 1000)
    {
        last_time = HAL_GetTick();
        print_log("[TOPIC] heartbeat");
        print_log("[TOPIC] time: %u", last_time);
    }
}
