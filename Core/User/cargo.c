#include "cargo.h"
#include "commands.h"

t_cargoContext cargo = {0};

uint32_t init_cargo(void) {
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
    // todo: set GPIO pin to enable electromagnet
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
