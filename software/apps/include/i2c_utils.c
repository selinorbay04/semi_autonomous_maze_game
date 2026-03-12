
#include "nrf.h"
#include "microbit_v2.h"
#include "nrf_error.h"
#include "nrf_twi_mngr.h"
#include "sdk_errors.h"
#include <stdint.h>
#include <stdio.h>

#include "state.h"
#include "i2c_utils.h"

// External i2c manager
extern const nrf_twi_mngr_t* state_i2c;
extern const nrf_twi_mngr_t* internal_i2c;

void i2c_init() {

    // init edge pin i2c bus

    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    printf("I'm running! (TWI)\n");
    i2c_config.scl = EDGE_P19;
    i2c_config.sda = EDGE_P20;
    i2c_config.frequency = NRF_DRV_TWI_FREQ_100K;
    i2c_config.interrupt_priority = 0;

    ret_code_t init_ret = nrf_twi_mngr_init(state_i2c, &i2c_config);
    if (init_ret != NRF_SUCCESS) {
        printf("some error: %i in init twi\n", init_ret);
    } else {
        printf("successful twi init\n");
    }

    // init internal pin i2c bus

    nrf_drv_twi_config_t i2c_config_internal = NRF_DRV_TWI_DEFAULT_CONFIG;
    // WARNING!!
    // These are NOT the correct pins for external I2C communication.
    // If you are using QWIIC or other external I2C devices, the are
    // connected to EDGE_P19 (a.k.a. I2C_QWIIC_SCL) and EDGE_P20 (a.k.a. I2C_QWIIC_SDA)
    i2c_config_internal.scl = I2C_INTERNAL_SCL;
    i2c_config_internal.sda = I2C_INTERNAL_SDA;
    i2c_config_internal.frequency = NRF_DRV_TWI_FREQ_100K;
    i2c_config_internal.interrupt_priority = 0;
    init_ret = nrf_twi_mngr_init(internal_i2c, &i2c_config_internal);
    if (init_ret != NRF_SUCCESS) {
        printf("some error: %i in init twi\n", init_ret);
    } else {
        printf("successful twi init\n");
    }
}

void i2c_write_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
    uint8_t combined_buffer[length];
    combined_buffer[0] = reg_addr;

    for (int i = 0; i < length; i++) {
        combined_buffer[i + 1] = buffer[i];
    }

    i2c_write_packet(i2c_addr, combined_buffer, length + 1, 0);
}

void i2c_read_reg(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
    i2c_write_byte(i2c_addr, &reg_addr, NRF_TWI_MNGR_NO_STOP);
    i2c_read_packet(i2c_addr, buffer, length, 0);
}

void i2c_write_reg_internal(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
    uint8_t combined_buffer[length];
    combined_buffer[0] = reg_addr;

    for (int i = 0; i < length; i++) {
        combined_buffer[i + 1] = buffer[i];
    }

    i2c_write_packet_internal(i2c_addr, combined_buffer, length + 1, 0);
}

void i2c_read_reg_internal(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t length) {
    i2c_write_byte_internal(i2c_addr, &reg_addr, NRF_TWI_MNGR_NO_STOP);
    i2c_read_packet_internal(i2c_addr, buffer, length, 0);
}

void i2c_manage_packet(uint8_t i2c_addr,
                       uint8_t *buffer,
                       uint8_t length,
                       uint8_t flags,
                       bool rx,
                       bool internal) {

    nrf_twi_mngr_transfer_t read_transfer =
        NRF_TWI_MNGR_READ(i2c_addr, buffer, length, flags);

    nrf_twi_mngr_transfer_t write_transfer =
        NRF_TWI_MNGR_WRITE(i2c_addr, buffer, length, flags);

    ret_code_t result;

    if (internal) {
        if (rx) {
            result = nrf_twi_mngr_perform(internal_i2c, NULL, &read_transfer, 1, NULL);
        } else {
            result = nrf_twi_mngr_perform(internal_i2c, NULL, &write_transfer, 1, NULL);
        }

    } else {
        if (rx) {
            result = nrf_twi_mngr_perform(state_i2c, NULL, &read_transfer, 1, NULL);
        } else {
            result = nrf_twi_mngr_perform(state_i2c, NULL, &write_transfer, 1, NULL);
        }
    }

    if (result != NRF_SUCCESS) {
        printf("I2C transaction failed! Error: %lx\n", result);
    }
}
