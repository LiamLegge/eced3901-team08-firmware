#include "logging.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static UART_HandleTypeDef *ros_uart = NULL;
static uint32_t loop_start;

static void profile_loop(uint32_t start_time)
{
    uint32_t loop_time = HAL_GetTick() - start_time;

    if (loop_time > 5)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "[ LOG ] Loop: %lu ms", loop_time);
        print_log(buf);
    }
}

void profile_begin(void) {
    loop_start = HAL_GetTick();
}



void profile_end(void) {
    profile_loop(loop_start);
}


void init_logging(UART_HandleTypeDef *huart) {
    ros_uart = huart;
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

void print_log(const char *fmt, ...) {
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