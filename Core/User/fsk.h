#pragma once

#include "stdint.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>

// Timer Handles
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim15;

// FSK commands, provided be UART
typedef enum {
    FSK_CMD_IDLE    = 0x00,
    FSK_CMD_START   = 0x01,
    FSK_CMD_STOP    = 0x02,
    FSK_CMD_TEST    = 0x03
} fskCommand_t;

// FSK Commands
void init_fsk(uint8_t* msg);
void start_fsk(void);
void stop_fsk(void);
int fsk_main(fskCommand_t cmd);
