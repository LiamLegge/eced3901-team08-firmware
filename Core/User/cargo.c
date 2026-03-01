#include "cargo.h"
#include "commands.h"
#include "adc.h"
#include "logging.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_gpio.h"

extern ADC_HandleTypeDef hadc1;

#define VERBOSE true

#define ADC_CH ADC1
#define ADC_HANDLE (&hadc1)
#define ADC_BUFFER_SIZE 64

#define EMAG_GPIO_PIN GPIO_PIN_1
#define EMAG_GPIO_PORT GPIOA

t_cargoContext cargo = {0};
uint16_t adc_buffer[ADC_BUFFER_SIZE] = {0}; // Buffer for ADC readings, size 1 for single channel
uint16_t adc_value = 0; // Latest ADC value

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min + 1) / (in_max - in_min + 1) + out_min;
}

uint16_t get_adc_value(void) { 
    return adc_value;
}

void print_adc_value(void) {
    if (VERBOSE) {
        print_log("[ ADC ] Current value: %u", get_adc_value());
    }
}

void print_status(HAL_StatusTypeDef st) {
    if (st != HAL_OK) {
        uint32_t adc_err = HAL_ADC_GetError(ADC_HANDLE);
        print_log("[ ADC ] Start_DMA failed: st=%d err=0x%08lx state=0x%08lx",
                (int)st, adc_err, (unsigned long)HAL_ADC_GetState(ADC_HANDLE));

        // Also check DMA error if handle exists
        if (ADC_HANDLE->DMA_Handle) {
            print_log("[ ADC ] DMA state=%d dma_err=0x%08lx",
                    (int)HAL_DMA_GetState(ADC_HANDLE->DMA_Handle),
                    (unsigned long)HAL_DMA_GetError(ADC_HANDLE->DMA_Handle));
        } else {
            print_log("[ ADC ] DMA_Handle is NULL (not linked)");
        }
        Error_Handler();
    }
}

void start_adc(void) {
    
    // Calibration
    if (VERBOSE) {
        print_log("[ ADC ] Calibrating...");
    }
    HAL_StatusTypeDef calib_status = HAL_ADCEx_Calibration_Start(ADC_HANDLE);
    print_status(calib_status);
    if (calib_status != HAL_OK) {
        Error_Handler();
    }
    
    HAL_StatusTypeDef adc_status = HAL_ADC_Start_DMA(ADC_HANDLE, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);
    print_status(adc_status);
    // Start ADC with DMA
    if (adc_status != HAL_OK) {
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

    uint16_t emag_voltage = get_adc_value();
    if (VERBOSE) { 
        print_log("[CARGO] EMAG voltage: %u", emag_voltage);
    }

    return ret;
}




void emag_callback(void) {
    // Called when cargo is detected by the current sensor.
    cargo.cargo_detected = true;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc != ADC_HANDLE) return;
    adc_value = adc_buffer[0];
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance != ADC_CH) return;

    // Callback for when the ADC goes outside the window
    emag_callback();
}

