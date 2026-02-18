#include "main.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOG_BUFFER_SIZE 128

static UART_HandleTypeDef *ros_uart = NULL;

void init_ros_topic(UART_HandleTypeDef *huart) {
    ros_uart = huart;
}

void ros_topic_main(void) {
    // Simple heartbeat
    
    static uint32_t last_time = 0;

    if (HAL_GetTick() - last_time >= 1000)
    {
        last_time = HAL_GetTick();
        log("[TOPIC] heartbeat");
        log("[TOPIC] time: %u", last_time);
    }
}

void send_uart_message(uint8_t *msg, uint16_t length) {
    if (ros_uart == NULL) {
        return;
    }
    HAL_UART_Transmit(ros_uart, (uint8_t *)msg, length, HAL_MAX_DELAY);
}

void send_byte(uint8_t byte) {
    if (ros_uart == NULL) {
        return;
    }
    HAL_UART_Transmit(ros_uart, &byte, 1, HAL_MAX_DELAY);
}

void log(const char *fmt, ...) {
    if (ros_uart == NULL) {
        return;
    }

    char buffer[LOG_BUFFER_SIZE];
    
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len < 0)
        return;

    if (len < (int)(sizeof(buffer) - 2)) {
        buffer[len++] = '\r';
        buffer[len++] = '\n';
        buffer[len] = '\0';
    }

    HAL_UART_Transmit(ros_uart,
                      (uint8_t *)buffer,
                      len,
                      HAL_MAX_DELAY);
}
