#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Initialize the LED matrix display
void led_matrix_init(void);

void read_char(char c);

void set_string(char* str);

void set_state(bool states[5][5]);

// You may need to add more functions here

