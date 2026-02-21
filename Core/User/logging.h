#pragma once

#include "usart.h"
#include <stdbool.h>


#define LOG_BUFFER_SIZE 128
#define PROFILE_VERBOSE false
#define PROFILE_ENABLE false

void profile_begin(void);
void profile_end(void);

void init_logging(UART_HandleTypeDef *huart);

void send_uart_message(uint8_t *msg, uint16_t length);

void send_ros_message(uint8_t *msg);
void send_byte(uint8_t byte);

void read_ros_message(void);

void print_log(const char *fmt, ...);
