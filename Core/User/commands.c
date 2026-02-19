#include "commands.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include"logging.h"
#include "stdio.h"

extern UART_HandleTypeDef UART_HANDLE;

volatile bool rxReady = false;
static uint8_t rxCopy[12];

static volatile uint8_t rxBuffer[12] = {0};
static volatile uint8_t rxByte; 

static void enable_rx_interrupt(void) {
    HAL_UART_Receive_IT(&UART_HANDLE, rxBuffer, sizeof(rxBuffer));
}

static void enable_rx_interrupt_byte(void) {
    HAL_UART_Receive_IT(&UART_HANDLE, (unsigned char*)&rxByte, 1);
}

void init_commands(void) {    
    rxReady = false;
    // enable_rx_interrupt();
    enable_rx_interrupt_byte();
}

int command_main(void) {
    if (!rxReady) {
        return 0;
    }
    
    rxReady = false;

    char currentCmd = rxByte;
    

    return currentCmd;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart != &UART_HANDLE) return;

    rxReady = true;

    char logBuffer[64];
    snprintf(logBuffer, sizeof(logBuffer),"[ CMD ] Char recieved: '%c'", rxByte);
    print_log(logBuffer);

    enable_rx_interrupt_byte();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart != &UART_HANDLE) return;

    // Overrun is the common "RX stops" problem
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_OREFLAG(huart);
    }

    // restart reception
    enable_rx_interrupt_byte();
}