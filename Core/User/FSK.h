#pragma once

#include "stdint.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim15;

<<<<<<< working-fsk
void init_fsk(uint8_t* msg);

void start_fsk(void);
=======
void init_fsk(uint8_t* msg, uint32_t *pwm_data);

void start_fsk(uint32_t *pwm_data);
>>>>>>> 18-feat-implement-fsk

void stop_fsk(void);
