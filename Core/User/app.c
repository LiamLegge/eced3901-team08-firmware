#include "app.h"
#include "ARGB.h"
#include "stm32g0b1xx.h"
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_tim.h"
#include "tim.h"
#include "FSK.h"
#include <stdint.h>

#define FRAMEDELAY 10

void app(void) {
    init_fsk((uint8_t*)"BYE");
    start_fsk();

    // main program loop
    for(;;) {
        start_fsk();
        HAL_Delay(100);
    }
}
