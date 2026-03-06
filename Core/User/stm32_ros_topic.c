#include "main.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"
#include "logging.h"


void ros_topic_main(void) {
    // Simple heartbeat
    static uint32_t last_time = 0;

    if (HAL_GetTick() - last_time >= HEARTBEAT_INTERVAL_MS)
    {
        last_time = HAL_GetTick();
        print_log("[TOPIC] heartbeat time: %u", last_time);
    }
}
