
#include "nrf.h"
#include <stdint.h>

#include "i2c_utils.h"

#define GRIDEYE_ADDR 0x69
#define TEMP_REG_START 0x80

// Given a pixel address 0, 63 return its reading as temp
float read_pixel(uint8_t pixel_addr);

// Given a buffer to write to, returns the whole pixel grid
void read_grid(float* grid_buffer);

// Debugging tool which serial prints the grid reading
// in a readable format
void print_grid();

// Debugging tool which prints which grid indices are
// considered "line" with a given threshold
void print_grid_threshold(float threshold);
