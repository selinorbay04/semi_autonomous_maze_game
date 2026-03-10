
#include "nrf.h"
#include "nrf_twi_mngr.h"
#include <stdint.h>

#include "state.h"

NRF_TWI_MNGR_DEF(i2c_def, 1, 0);

const nrf_twi_mngr_t* state_i2c = &i2c_def;

bool state_at_junction = false;
bool state_left_triggered = false;
bool state_right_triggered = false;

uint8_t state_avg_buffer_index = 0;
uint16_t state_avg_buffer[STATE_AVG_BUFFER_SIZE];
