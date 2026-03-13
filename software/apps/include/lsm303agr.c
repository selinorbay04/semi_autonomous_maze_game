

#include "lsm303agr.h"
#include "i2c_utils.h"
#include "nrf_delay.h"
#include <stdint.h>
#include <stdio.h>

void lsm303agr_init() {

    // INIT MAG

    uint8_t reboot_byte = 0x40;
    uint8_t block_update_byte = 0x10;
    uint8_t config_byte = 0x0c;

    i2c_write_reg_internal(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, &reboot_byte, 1);
    nrf_delay_ms(100);
    i2c_write_reg_internal(LSM303AGR_MAG_ADDRESS, CFG_REG_C_M, &block_update_byte, 1);
    i2c_write_reg_internal(LSM303AGR_MAG_ADDRESS, CFG_REG_A_M, &config_byte, 1);

    uint8_t whoami_byte = 0;
    i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, WHO_AM_I_M, &whoami_byte, 1);

    printf("WHOAMI: %i\n", whoami_byte);
}

float get_heading() {
    //uint8_t x_l = 0;
    //uint8_t x_h = 0;
    uint8_t y_l = 0;
    uint8_t y_h = 0;
    //uint8_t z_l = 0;
    //uint8_t z_h = 0;

    //i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTX_L_REG_M, &x_l, 1);
    //i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTX_H_REG_M, &x_h, 1);
    i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTY_L_REG_M, &y_l, 1);
    i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTY_H_REG_M, &y_h, 1);
    //i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTZ_L_REG_M, &z_l, 1);
    //i2c_read_reg_internal(LSM303AGR_MAG_ADDRESS, OUTZ_H_REG_M, &z_h, 1);

    //int16_t x_raw = (((uint16_t)x_h << 8) | x_l);
    int16_t y_raw = (((uint16_t)y_h << 8) | y_l);
    //int16_t z_raw = (((uint16_t)z_h << 8) | z_l);

    //float x_scaled = (float) x_raw * 1.5 / 10.0;
    float y_scaled = (float) y_raw * 1.5 / 10.0;
    //float z_scaled = (float) z_raw * 1.5 / 10.0;

    //lsm303agr_measurement_t measurement = {
    //    x_scaled,
    //    y_scaled,
    //    z_scaled
    //};

    //printf("x: %f\n", x_scaled);
    printf("y: %f\n", y_scaled);
    //printf("z: %f\n\n", z_scaled);

    return 0;
}
