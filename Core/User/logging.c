#include "logging.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static UART_HandleTypeDef *ros_uart = NULL;
static uint32_t loop_start;


static void profile_loop(uint32_t start_time)
{
    uint32_t loop_time = HAL_GetTick() - start_time;

    if (loop_time > 5) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[ LOG ] Loop (ms):     %lu ", (unsigned long)loop_time);
        print_log(buf);
    }
}

void profile_begin(void) {
    char buf[64];

    loop_start = HAL_GetTick();
    
    if (PROFILE_VERBOSE) {
        print_log("[ LOG ] Starting profiler...");
        snprintf(buf, sizeof(buf),          "[ LOG ] Start (ms):   %lu", (unsigned long)loop_start);
        print_log(buf);
    }
}



void profile_end(void) {
    char buf[64];
    profile_loop(loop_start);
    if (PROFILE_VERBOSE) {
        print_log("[ LOG ] Ending profiler");
        snprintf(buf, sizeof(buf),          "[ LOG ] End (ms):      %lu", (unsigned long)HAL_GetTick());
        print_log(buf);
    }
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
