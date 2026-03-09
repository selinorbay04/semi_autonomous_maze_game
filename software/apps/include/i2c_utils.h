
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"
#include <stdint.h>


ret_code_t i2c_init(const nrf_twi_mngr_t* i2c);

void i2c_write_byte(uint8_t i2c_addr, uint8_t byte);

void i2c_read_byte(uint8_t i2c_addr, uint8_t* buffer);

void i2c_write_packet(uint8_t i2c_addr, uint8_t* payload);

void i2c_read_packet(uint8_t i2c_addr, uint8_t* buffer);

void 