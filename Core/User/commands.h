#pragma once

typedef enum
{
    CMD_NONE = 0,
    CMD_RUN,
    CMD_HALT,
    CMD_COUNT        // always keep last
} Command_t;


void init_commands(void);
int command_main(void);
