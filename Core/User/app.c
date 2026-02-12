
#include "app.h"
#include "stm32g0xx_hal.h"

/* Module headers */
// #include "led.h"
// #include "fsk.h"
// #include "sensor.h"
// #include "cargo.h"

__weak void init_led(void)    {}
__weak void led_main(void)    {}

__weak void init_fsk(void)    {}
__weak void fsk_main(void)    {}

__weak void init_sensor(void) {}
__weak void sensor_main(void) {}

__weak void init_cargo(void)  {}
__weak void cargo_main(void)  {}


void app_init(void)
{
    init_led();
    init_fsk();
    init_sensor();
    init_cargo();
}

/* ============================================================
 * Application main loop
 * ============================================================ */

void app(void)
{

    app_init();
    
    for (;;)
    {
        led_main();
        fsk_main();
        sensor_main();
        cargo_main();

        /* Optional: cooperative scheduling yield */
        // HAL_Delay(1);
    }
}
