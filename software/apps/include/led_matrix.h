

#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>



#define RIGHT_ARROW {{0,0,1,0,0}, \
                    {0,0,0,1,0}, \
                    {1,1,1,1,1}, \
                    {0,0,0,1,0}, \
                    {0,0,1,0,0}}

#define LEFT_ARROW  {{0,0,1,0,0}, \
                    {0,1,0,0,0}, \
                    {1,1,1,1,1}, \
                    {0,1,0,0,0}, \
                    {0,0,1,0,0}}

#define SMILEY_STATE {{0,1,0,1,0}, \
                    {0,1,0,1,0}, \
                    {0,0,0,0,0}, \
                    {1,0,0,0,1}, \
                    {0,1,1,1,0}}

#define Q_STATE     {{0,1,1,0,0}, \
                    {1,0,0,1,0}, \
                    {0,0,1,0,0}, \
                    {0,0,0,0,0}, \
                    {0,0,1,0,0}}

#define X_STATE     {{1,0,0,0,1}, \
                    {0,1,0,1,0}, \
                    {0,0,1,0,0}, \
                    {0,1,0,1,0}, \
                    {1,0,0,0,1}}





void led_matrix_init(void);

void set_state(bool states[5][5]);
void led_matrix_set_frame(bool frame[5][5]);

// Start blinking the given frame 
void led_matrix_blink_start(bool frame[5][5]);

// Stop blinking and clear the display
void led_matrix_blink_stop(void);




