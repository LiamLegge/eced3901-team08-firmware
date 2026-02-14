#include "main.h"
#include "stm32_ros_topic.h"
#include "stm32g0xx_hal_gpio.h"

static UART_HandleTypeDef *ros_uart = NULL;

void init_ros_topic(UART_HandleTypeDef *huart) {
    ros_uart = huart;
}


void ros_topic_main(void) {
    // Simple heartbeat
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    uint8_t byte = 0x55;
    send_byte(byte);
}

void send_ros_message(uint8_t msg[]) {
    (void)msg;
}

void send_byte(uint8_t byte) {
    if (ros_uart == NULL) {
        return;
    }
    HAL_UART_Transmit(ros_uart, &byte, 1, HAL_MAX_DELAY);
}


