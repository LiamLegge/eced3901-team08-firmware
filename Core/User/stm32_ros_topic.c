#include "main.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"

static UART_HandleTypeDef *ros_uart = NULL;

void init_ros_topic(UART_HandleTypeDef *huart) {
    ros_uart = huart;
}


void ros_topic_main(void) {
    // Simple heartbeat
    uint8_t msg[] = "heartbeat\r\n";
    send_uart_message(msg, sizeof(msg) - 1);
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


