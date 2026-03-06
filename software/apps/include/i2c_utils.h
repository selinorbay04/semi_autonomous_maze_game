
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"
#include <stdint.h>


ret_code_t i2c_init(const nrf_twi_mngr_t* i2c);

#define i2c_write_byte(i2c_addr, buffer, flags) \
    i2c_write_packet(i2c_addr, buffer, 1, flags)

#define i2c_read_byte(i2c_addr, buffer, flags) \
    i2c_read_packet(i2c_addr, buffer, 1, flags)

#define i2c_write_packet(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, false)

#define i2c_read_packet(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, true)

void i2c_manage_packet(uint8_t i2c_addr,
                       uint8_t* buffer,
                       uint8_t length,
                       uint8_t flags,
                       bool rx);
