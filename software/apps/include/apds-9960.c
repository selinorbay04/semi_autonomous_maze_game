
#include "nrf_delay.h"

#include "apds-9960.h"
#include "i2c_utils.h"
#include "state.h"

#include <stdbool.h>
#include <stdint.h>

static uint8_t APDS_ENABLE_BYTE = 0b00000011;
static uint8_t APDS_DISABLE_BYTE = 1;
static uint8_t APDS_ATIME_BYTE = 0x00;
static uint8_t APDS_WTIME_BYTE = 0xff;
static uint8_t APDS_AGAIN_BYTE = 0x00;
static uint8_t APDS_CONFIG1_BYTE = 0x00;

void apds_init() {
    // Initialize the als engine
    i2c_write_reg(0x39, APDS_ATIME_ADDR, &APDS_ATIME_BYTE, 1);
    i2c_write_reg(0x39, APDS_WTIME_ADDR, &APDS_WTIME_BYTE, 1);
    i2c_write_reg(0x39, APDS_CONTROL_ADDR, &APDS_AGAIN_BYTE, 1);
    i2c_write_reg(0x39, APDS_CONFIG1_ADDR, &APDS_CONFIG1_BYTE, 1);
    i2c_write_reg(0x39, APDS_ENABLE_ADDR, &APDS_ENABLE_BYTE, 1);
}

void apds_shutdown() {
    i2c_write_reg(0x39, APDS_ENABLE_ADDR, &APDS_DISABLE_BYTE, 1);
}

bool check_avalid() {
    uint8_t status = 0;
    i2c_read_byte(0x39, &status, 0);

    // printf("Status: %x\n", status);

    return !(status & AVALID_MASK);
}

bool check_over_line(int threshold) {
    uint16_t cur_read = read_color();
    uint16_t avg_read = 0;

    for (int i = 0; i < STATE_AVG_BUFFER_SIZE; i++) {
        avg_read += state_avg_buffer[i];
    }

    avg_read /= STATE_AVG_BUFFER_SIZE;

    return cur_read + threshold < avg_read;
}

uint16_t read_color() {
    // Wait for AVALID bit
    while (check_avalid()) {
        //nrf_delay_ms(10);
    }

    // Read from CDATA
    uint8_t cdata_l;
    uint8_t cdata_h;
    i2c_read_reg(0x39, APDS_CDATAL_ADDR, &cdata_l, 1);
    i2c_read_reg(0x39, APDS_CDATAH_ADDR, &cdata_h, 1);
    uint16_t cdata = ((uint16_t)cdata_h << 8) | (uint16_t)cdata_l;

    //printf("dl: %x dh: %x\n", cdata_l, cdata_h);
    // Store the reading in the rolling avg buffer
    state_avg_buffer_index = (state_avg_buffer_index + 1) % STATE_AVG_BUFFER_SIZE;
    state_avg_buffer[state_avg_buffer_index] = cdata;

    return cdata;
    //return cdata_scaled;
}

void update_line_state(int threshold) {
    // TODO: Might need to go in the mux helper actually
}

