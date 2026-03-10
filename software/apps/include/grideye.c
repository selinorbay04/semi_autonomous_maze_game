
#include <stdint.h>
#include <stdio.h>

#include "i2c_utils.h"
#include "grideye.h"

float read_pixel(uint8_t pixel_addr) {
    uint8_t pixel_reg = TEMP_REG_START + (2 * pixel_addr);

    uint8_t pixel_buffer[2];
    i2c_read_reg(GRIDEYE_ADDR, pixel_reg, pixel_buffer, 2);

    int16_t pixel_raw = ((int16_t)pixel_buffer[1] << 8) | pixel_buffer[0];
    float pixel_temp = (float)pixel_raw * 0.25;

    return pixel_temp;
}

void read_grid(float *grid_buffer) {
    for (int i = 0; i < 64; i++) {
        grid_buffer[i] = read_pixel(i);
    }
}

void print_grid() {
    float grid[64];
    read_grid(grid);

    printf("\n");
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            printf("[%02.2f] ", grid[row*8 + col]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_grid_threshold(float threshold) {
    float grid[64];
    read_grid(grid);
    float average_val = 0;

    for (int i = 0; i < 64; i++) {
        average_val += grid[i];
    }

    average_val /= 64;

    printf("\n");
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            float cur_val = grid[row*8 + col];
            if (cur_val > average_val + threshold) {
                printf("[X] ");
            } else {
                printf("[ ] ");
            }
        }
        printf("\n");
    }
    printf("\n");
}
