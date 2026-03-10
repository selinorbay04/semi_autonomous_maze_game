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
} APDS_ADDR;

#define AVALID_MASK 1
#define AGAIN_MASK 3

// Init/shutdown the device for color reads
void apds_init();
void apds_shutdown();

// Read the clear color channel
uint16_t read_color();

// Return if the sensor is over a line
// Given a threshold to ignore small variations
bool check_over_line(int threshold);

// Update the state flags about being over the line
// Given a threshold to ignore small variations
// TODO: Might need to go in the mux helper actually
void update_line_state(int threshold);

// Below this line probably not worth implementing
// ------------------------------------------------

// Enable/Disable ALS interrupts
void enable_als_interrupts();
void disable_als_interrupts();

// Set the thresholds for the als interrupt
void set_als_thresholds(uint16_t low, uint16_t high);

#endif
