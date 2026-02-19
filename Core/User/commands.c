#include "commands.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include"logging.h"
#include "stdio.h"

#define RX_DMA_BUF_SIZE 128
#define CMD_MAX_LEN 64

extern UART_HandleTypeDef UART_HANDLE;

volatile bool rxReady = false;

static uint8_t rxBuffer[12] = {0};
static uint8_t rxByte; 

static void enable_rx_interrupt(void) {
    HAL_UART_Receive_IT(&UART_HANDLE, rxBuffer, sizeof(rxBuffer));
}

static void enable_rx_interrupt_byte(void) {
    HAL_UART_Receive_IT(&UART_HANDLE, (unsigned char*)&rxByte, 1);
}

static void start_rx_to_idle_dma(void)
{
    // Start a DMA reception that completes on IDLE or buffer full
    HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLE, rxBuffer, RX_DMA_BUF_SIZE);

    // Optional: disable Half Transfer IRQ to reduce interrupt spam
    if (UART_HANDLE.hdmarx) {
        __HAL_DMA_DISABLE_IT(UART_HANDLE.hdmarx, DMA_IT_HT);
    }
}

void init_commands(void) {    
    rxReady = false;
    // enable_rx_interrupt();
    // enable_rx_interrupt_byte();
    start_rx_to_idle_dma();
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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart != &UART_HANDLE) return;

    // 'size' bytes are valid in rx_dma_buf[0..size-1]
    // for (uint16_t i = 0; i < size; i++) {
    //     rb_push_byte(rx_dma_buf[i]);
    // }

    // (Optional) log how many bytes arrived
    print_log("[CMD] RX burst: %u bytes", (unsigned)size);

    // Restart for next burst
    start_rx_to_idle_dma();
}
