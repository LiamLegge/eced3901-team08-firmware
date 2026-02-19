#pragma once

#include "usart.h"

#define UART_HANDLE huart2
#define CMD_MAX_LEN 64

void init_commands(void);
int command_main(void);
