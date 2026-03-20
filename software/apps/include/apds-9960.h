#ifndef APDS_H
#define APDS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    APDS_ENABLE_ADDR = 0x80,
    APDS_ATIME_ADDR = 0x81,
    APDS_WTIME_ADDR = 0x83,
    APDS_CONFIG1_ADDR = 0x8d,
    APDS_CONTROL_ADDR = 0x8f,
    APDS_STATUS_ADDR = 0x93,
    APDS_CDATAL_ADDR = 0x94,
    APDS_CDATAH_ADDR = 0x95,
    APDS_RDATAL_ADDR = 0x96,
    APDS_RDATAH_ADDR = 0x97,
    APDS_GDATAL_ADDR = 0x98,
    APDS_GDATAH_ADDR = 0x99,
    APDS_BDATAL_ADDR = 0x9a,
    APDS_BDATAH_ADDR = 0x9b,
} APDS_ADDR;

typedef enum {
    RED,
    GREEN,
    BLUE,
} COLOR;

#define AVALID_MASK 1
#define AGAIN_MASK 3

// Init/shutdown the device for color reads
void apds_init(bool left);
void apds_shutdown();

// Read the clear/r/g/b color channel
#define read_clear() read_color(APDS_CDATAL_ADDR, APDS_CDATAH_ADDR)
#define read_red() read_color(APDS_RDATAL_ADDR, APDS_RDATAH_ADDR)
#define read_green() read_color(APDS_GDATAL_ADDR, APDS_GDATAH_ADDR)
#define read_blue() read_color(APDS_BDATAL_ADDR, APDS_BDATAH_ADDR)
uint16_t read_color(uint8_t l_reg, uint8_t h_reg);

// Takes in a color of interest (R/G/B) and a threshold
// returns true if the color is greater than
// all the others by more than the threshold
bool detect_color(COLOR COI, float threshold);

// Return if the sensor is over a line
// Given a threshold to ignore small variations
bool check_over_line(bool left);

// Below this line probably not worth implementing
// ------------------------------------------------

// Enable/Disable ALS interrupts
void enable_als_interrupts();
void disable_als_interrupts();

// Set the thresholds for the als interrupt
void set_als_thresholds(uint16_t low, uint16_t high);

#endif
