#pragma once 

#include "stdbool.h"
#include "stdint.h"

#include "usart.h"


void init_ros_topic(UART_HandleTypeDef *huart);
void ros_topic_main(void);

void stm32_send_msg(uint8_t byte);

/* Commands for firmware functions */

void send_uart_message(uint8_t *msg, uint16_t length);

void send_ros_message(uint8_t *msg);
void send_byte(uint8_t byte);


void read_ros_message(void);
