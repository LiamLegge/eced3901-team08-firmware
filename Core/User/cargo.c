#include "cargo.h"
#include "commands.h"
#include "adc.h"
#include "logging.h"
#include "stm32g0xx_hal_gpio.h"

extern ADC_HandleTypeDef hadc1;

#define ADC_CH ADC1
#define ADC_HANDLE (&hadc1)
#define EMAG_GPIO_PIN GPIO_PIN_1
#define EMAG_GPIO_PORT GPIOA
#define VERBOSE true



t_cargoContext cargo = {0};

void start_adc(void) {
    // Calibration (untested)
    if (VERBOSE) {
        print_log("[ ADC ] Calibrating...");
    }

    if (HAL_ADCEx_Calibration_Start(ADC_HANDLE) != HAL_OK) {
        Error_Handler();
    }

    // Start ADC with interrupts enabled.
    if (HAL_ADC_Start_IT(ADC_HANDLE) != HAL_OK) {
        Error_Handler();
    }
}


uint32_t init_cargo(void) {   
    start_adc();
    cargo.emag_en = false;
    cargo.cargo_detected = false;
    return 0;
}

bool cargo_detected(void) {
    if (cargo.cargo_detected) {
        return true;
    }
    return false;
}

void enable_emag(void) {
    cargo.emag_en = true;
    HAL_GPIO_WritePin(EMAG_GPIO_PORT, EMAG_GPIO_PIN, GPIO_PIN_SET);
}

void disable_emag(void) {
    cargo.emag_en = false;
    // todo: set GPIO pin to disable electromagnet
}

void toggle_emag(void) {
    if (cargo.emag_en) {
        disable_emag();
    } else {
        enable_emag();
    }
}

uint32_t cargo_main(uint16_t cmd) {
    uint32_t ret = 0x00; 
    switch (cmd) {
        case CMD_EMAG_ENABLE: enable_emag(); break;
        case CMD_EMAG_DISABLE: disable_emag(); break;
        case CMD_EMAG_TOGGLE: toggle_emag(); break;
        default: break;
    }
    return ret;
}


void emag_callback(void) {
    // Called when cargo is detected by the current sensor.
    cargo.cargo_detected = true;
}


void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance != ADC_CH) return;

    // Callback for when the ADC goes outside the window
    emag_callback();
}
