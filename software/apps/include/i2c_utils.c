
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"
#include <stdint.h>
#include <stdio.h>

// Pointer to initialized i2c manager
static const nrf_twi_mngr_t* i2c_manager = NULL;

void i2c_init(const nrf_twi_mngr_t* i2c) {
    i2c_manager = i2c;
}


void i2c_manage_packet(uint8_t i2c_addr,
                       uint8_t *buffer,
                       uint8_t length,
                       uint8_t flags,
                       bool rx) {

    nrf_twi_mngr_transfer_t read_transfer =
        NRF_TWI_MNGR_READ(i2c_addr, buffer, length, flags);

    nrf_twi_mngr_transfer_t write_transfer =
        NRF_TWI_MNGR_WRITE(i2c_addr, buffer, length, flags);

    ret_code_t result;

    if (rx) {
        result = nrf_twi_mngr_perform(i2c_manager, NULL, &read_transfer, 1, NULL);
    } else {
        result = nrf_twi_mngr_perform(i2c_manager, NULL, &write_transfer, 1, NULL);
    }

    if (result != NRF_SUCCESS) {
        printf("I2C transaction failed! Error: %lx\n", result);
    }
}
