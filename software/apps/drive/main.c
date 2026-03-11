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

  auto_drive(100, 100, 10);

  // Enter main loop.
  // while (1) {
  //   // uint16_t read_val = read_color();
  //   // printf("Current reading: %u\n", read_val);
  //   printf("Currently over line? : %d\n", check_over_line(7));
  //   nrf_delay_ms(250);
  // }
}

