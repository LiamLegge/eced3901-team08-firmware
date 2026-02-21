#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool emag_en;
    bool cargo_detected;
 } t_cargoContext;

uint32_t init_cargo(void);
uint32_t cargo_main(uint16_t cmd);
