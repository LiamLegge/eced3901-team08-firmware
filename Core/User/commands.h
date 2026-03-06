#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CMD_NONE            = 0x00,
    CMD_RUN             = 0x01,
    CMD_HALT            = 0x02,
    CMD_EMAG_ENABLE     = 0xf1,
    CMD_EMAG_DISABLE    = 0xf2,
    CMD_EMAG_TOGGLE     = 0xf3
} t_command;


void init_commands(void);
int command_main(void);
