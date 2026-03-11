
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

void apds_init() {
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
        avg_read += read_color();
    }

    avg_read /= num_reads;
    state_avg_white_reading = avg_read;
    printf("average read is: %i\n", state_avg_white_reading);
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

bool check_over_line(int threshold) {
    uint16_t cur_read = read_color();

    return cur_read + threshold < state_avg_white_reading;
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
    //printf("Current color: %i\n", cdata);

    return cdata;
    //return cdata_scaled;
}

void update_line_state(int threshold) {
    // TODO: Might need to go in the mux helper actually
    // For now I'm just putting kinda working stuff
    // to test the autodrive function
    if (check_over_line(threshold) && state_line_trigger != STATE_LEFT_TRIGGERED) {
        printf("Line state says should start turn\n");
        state_line_trigger = STATE_LEFT_TRIGGERED;
        state_line_changed = true;
    } else if (!check_over_line(threshold) && state_line_trigger != STATE_NO_TRIGGERS) {
        printf("Line state says should start drive\n");
        state_line_trigger = STATE_NO_TRIGGERS;
        state_line_changed = true;
    }
}

