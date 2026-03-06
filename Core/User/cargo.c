#include "cargo.h"
#include "commands.h"
#include "adc.h"
#include "logging.h"
#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_gpio.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

#define VERBOSE true
#define WINDOW_VERBOSE false
#define VOLTAGE_VERBOSE false

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

// Check if value is outside the window defined by low and high
// Returns true if value is outside the window, false if inside
bool window(uint16_t value, uint16_t low, uint16_t high) {
    if (value < low) {
        if (WINDOW_VERBOSE) {
            print_log("[CARGO] Value %u is below low threshold %u", value, low);
        }
    } else if (value > high) {
        if (WINDOW_VERBOSE) {
            print_log("[CARGO] Value %u is above high threshold %u", value, high);
        }
    } else {
        if (WINDOW_VERBOSE) {
            print_log("[CARGO] Value %u is within thresholds (%u - %u)", value, low, high);
        }
    }
    return (value < low || value > high);
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
    if (VERBOSE) {
        print_log("[CARGO] Electromagnet enabled");
    }
}

void disable_emag(void) {
    cargo.emag_en = false;
    HAL_GPIO_WritePin(EMAG_GPIO_PORT, EMAG_GPIO_PIN, GPIO_PIN_RESET);
    if (VERBOSE) {
        print_log("[CARGO] Electromagnet disabled");
    }
}

void toggle_emag(void) {
    if (cargo.emag_en) {
        disable_emag();
    } else {
        enable_emag();
    }
    if (VERBOSE) {
        print_log("[CARGO] Electromagnet toggled to %s", cargo.emag_en ? "ON" : "OFF");
    }
}

bool is_cargo_detected(uint16_t emag_voltage)
{
    const uint16_t LOW_THRESHOLD  = 652;
    const uint16_t HIGH_THRESHOLD = 960;

    bool was_detected = cargo.cargo_detected;

    // Decide new state
    bool now_detected = window(emag_voltage, LOW_THRESHOLD, HIGH_THRESHOLD);
    if (now_detected != was_detected) {
        cargo.cargo_detected = now_detected;

        // Print only on transition
        if (now_detected) {
            print_log("[CARGO] LEFT window (v=%u, win=[%u..%u])",
                    (unsigned)emag_voltage,
                    (unsigned)LOW_THRESHOLD,
                    (unsigned)HIGH_THRESHOLD);
        } else {
            print_log("[CARGO] ENTERED window (v=%u, win=[%u..%u])",
                    (unsigned)emag_voltage,
                    (unsigned)LOW_THRESHOLD,
                    (unsigned)HIGH_THRESHOLD);
        }
    }

    return cargo.cargo_detected;
}

uint32_t cargo_main(uint16_t cmd) {
    // Cargo system commands
    switch (cmd) {
        case CMD_EMAG_ENABLE: enable_emag(); break;
        case CMD_EMAG_DISABLE: disable_emag(); break;
        case CMD_EMAG_TOGGLE: toggle_emag(); break;
        default: break;
    }

    // Check cargo status based on ADC value
    // A current shunt is used to measure the current through the electromagnet,
    // which correlates to the voltage measured by the ADC. When cargo is present,
    // the current will *briefly* spike as when the magnet attaches to the cargo.    
    uint16_t emag_voltage = get_adc_value();
    bool detected = is_cargo_detected(emag_voltage); // Only triggers on transitions
    return (uint32_t)detected;
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

