#ifndef STATE_H
#define STATE_H

#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>

#define STATE_AVG_BUFFER_SIZE 20

extern const nrf_twi_mngr_t* state_i2c;

extern bool state_at_junction;
extern bool state_left_triggered;
extern bool state_right_triggered;

extern uint8_t state_avg_buffer_index;
extern uint16_t state_avg_buffer[];

#endif
