
#include "nrf.h"
#include "nrf_twi_mngr.h"

NRF_TWI_MNGR_DEF(i2c_def, 1, 0);

const nrf_twi_mngr_t* i2c = &i2c_def;
