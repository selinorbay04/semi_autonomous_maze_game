
#include "nrf.h"
#include "nrf_twi_mngr.h"
#include <stdint.h>

#include "state.h"

NRF_TWI_MNGR_DEF(i2c_def, 1, 0);

const nrf_twi_mngr_t* state_i2c = &i2c_def;

bool state_game_over = false;

line_trigger state_line_trigger = STATE_NO_TRIGGERS;
bool state_line_changed = false;

uint16_t state_avg_white_reading = 0;
