#ifndef PCA_H
#define PCA_H

#include <stdint.h>

#define PCA_ADDR 0x70
#define MUX_LEFT 0b00000001
#define MUX_RIGHT 0b10000000

void mux_set_active(uint8_t ctrl_byte);

void mux_activate_left();
void mux_activate_right();

void mux_init_sensors();

// Update the state flags about being over the line
// Given a threshold to ignore small variations
void mux_update_line_state(int threshold);

#endif
