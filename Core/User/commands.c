#include "commands.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include"logging.h"
#include "stdio.h"

#define VERBOSE true

#define RX_DMA_BUF_SIZE 128
#define CMD_MAX_LEN 64

extern UART_HandleTypeDef UART_HANDLE;

volatile bool rxReady = false;

static volatile uint8_t rxBuffer[RX_DMA_BUF_SIZE] = {0};

static void clear_dma_buffer(void) {
    for (size_t i = 0; i < RX_DMA_BUF_SIZE; i++) {
        rxBuffer[i] = 0;
    }
} 

volatile uint8_t *rx_get_buffer(void) {
    return rxBuffer;
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
    start_rx_to_idle_dma();
}

// Converts a raw serial buffer to an array
// Clears the array passed to it
static void read_buffer(char *dest, size_t max_len)
{
    volatile uint8_t *src = rx_get_buffer();
       
    size_t i;
    for (i = 0; i < max_len - 1; i++)
    {
        uint8_t b = src[i];
        if (b == '\0' || b == '\n' || b == '\r')
            break;

        dest[i] = (char)b;
    }
    dest[i] = '\0';
}


int command_main(void) {
    if (!rxReady) {
        return 0;
    }
    
    rxReady = false;
    char cmd[RX_DMA_BUF_SIZE + 1] = {0};
    read_buffer(cmd, sizeof(cmd));
    clear_dma_buffer();
    // todo: check if `cmd` matches a command enum
    print_log("[ CMD ] %s", cmd);


    return 0; // todo: return a command
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart != &UART_HANDLE) return;

    rxReady = true;
    if (VERBOSE) {
        print_log("[ CMD ] RX burst: %u bytes", (unsigned)size);
    }

    // Restart for next burst
    start_rx_to_idle_dma();
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart != &UART_HANDLE) return;

    // Overrun is the common "RX stops" problem
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_OREFLAG(huart);
    }

    // restart reception
    start_rx_to_idle_dma();
}
