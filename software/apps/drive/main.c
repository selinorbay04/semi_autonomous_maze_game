// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"

// Helper libraries
#include "i2c_utils.h"
#include "car_utils.h"
#include "apds-9960.h"
#include "pca9548a.h"
// #include "radio_utils.h"

int main(void) {

  i2c_init();
  motor_init();
  mux_init_sensors();
  // nrf_802154_configure(true);

  //auto_drive(100, 100, 10);

  while (1) {
    uint16_t clear_val = read_clear();
    uint16_t red_val = read_red();
    uint16_t green_val = read_green();
    uint16_t blue_val = read_blue();

    printf("C: %i, R: %i, G: %i, B: %i\n", clear_val, red_val, green_val, blue_val);

    bool red_obj = detect_color(RED, 50);
    bool green_obj = detect_color(GREEN, 50);
    bool blue_obj = detect_color(BLUE, 50);

    printf("Looking at a red object?: %i\n", red_obj);
    printf("Looking at a green object?: %i\n", green_obj);
    printf("Looking at a blue object?: %i\n\n", blue_obj);

    nrf_delay_ms(250);
  }
}

