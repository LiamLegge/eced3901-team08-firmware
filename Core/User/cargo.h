#pragma once

#include <stdbool.h>
#include <stdint.h>

#define ADC_CHANNEL ADC_CHANNEL_1

typedef struct {
    bool emag_en;
    bool cargo_detected;
 } t_cargoContext;

uint32_t init_cargo(void);
uint32_t cargo_main(uint16_t cmd);
void emag_callback(void);
uint16_t get_adc_value(void);
void print_adc_value(void);
