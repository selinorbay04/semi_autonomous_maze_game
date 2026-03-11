#ifndef STATE_H
#define STATE_H

#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>

extern const nrf_twi_mngr_t* state_i2c;

extern bool state_game_over;

typedef enum {
    STATE_AT_JUNCTION,
    STATE_LEFT_TRIGGERED,
    STATE_RIGHT_TRIGGERED,
    STATE_NO_TRIGGERS,
} line_trigger;

extern line_trigger state_line_trigger;
extern bool state_line_changed;

extern uint16_t state_avg_white_reading;

#endif
