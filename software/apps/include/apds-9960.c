
#include "nrf_delay.h"

#include "apds-9960.h"
#include "i2c_utils.h"
#include "state.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t APDS_ENABLE_BYTE = 0b00000011;
static uint8_t APDS_DISABLE_BYTE = 1;
static uint8_t APDS_ATIME_BYTE = 0xf8;
static uint8_t APDS_WTIME_BYTE = 0x00;
static uint8_t APDS_CONFIG1_BYTE = 0b01100010;
static uint8_t APDS_CONTROL_BYTE = 0b11000010;

void apds_init(bool left) {
    // Initialize the als engine
    i2c_write_reg(0x39, APDS_ATIME_ADDR, &APDS_ATIME_BYTE, 1);
    i2c_write_reg(0x39, APDS_WTIME_ADDR, &APDS_WTIME_BYTE, 1);
    i2c_write_reg(0x39, APDS_CONTROL_ADDR, &APDS_CONTROL_BYTE, 1);
    i2c_write_reg(0x39, APDS_CONFIG1_ADDR, &APDS_CONFIG1_BYTE, 1);
    i2c_write_reg(0x39, APDS_ENABLE_ADDR, &APDS_ENABLE_BYTE, 1);

    // Get the average color on startup, assuming we
    // are not on top of a line and consider this
    // to be "white"
    uint8_t num_reads = 40;
    uint16_t avg_read = 0;

    for (int i = 0; i < num_reads; i++) {
        avg_read += read_clear();
    }

    avg_read /= num_reads;
    if (left) {
        state_left_avg_white_reading = avg_read;
    } else {
        state_right_avg_white_reading = avg_read;
    }
    printf("average read is: %i\n", avg_read);
}

void apds_shutdown() {
    i2c_write_reg(0x39, APDS_ENABLE_ADDR, &APDS_DISABLE_BYTE, 1);
}

bool check_avalid() {
    uint8_t status = 0;
    i2c_read_reg(0x39, APDS_STATUS_ADDR, &status, 1);

    // printf("Status: %x\n", status);

    return !(status & AVALID_MASK);
}

bool check_over_line(bool left) {
    uint16_t cur_read = read_clear();

    if (left) {
        return cur_read < state_left_avg_white_reading * state_line_threshold;
    } else {
        return cur_read < state_right_avg_white_reading * state_line_threshold;
    }
}

uint16_t read_color(uint8_t l_reg, uint8_t h_reg) {
    // Wait for AVALID bit
    while (check_avalid()) {
        // Spin
    }

    // Read from CDATA
    uint8_t cdata_l;
    uint8_t cdata_h;
    i2c_read_reg(0x39, l_reg, &cdata_l, 1);
    i2c_read_reg(0x39, h_reg, &cdata_h, 1);
    uint16_t cdata = ((uint16_t)cdata_h << 8) | (uint16_t)cdata_l;

    return cdata;
}

bool detect_color(COLOR COI, float threshold) {
    float r = (float)read_red();
    float g = (float)read_green();
    float b = (float)read_blue();

    //printf("R: %i, G: %i, B: %i\n", r, g, b);

    switch (COI) {
        case RED:
            return r > g * threshold && r > b * threshold;
        case GREEN:
            return g > r * threshold && g > b * threshold;
        case BLUE:
            return b > g * threshold && b > r * threshold;
    }

    return false;
}

