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
#include "pca9548a.h"
#include "hc-sr04.h"
#include "lsm303agr.h"
#include "apds-9960.h"
// #include "radio_utils.h"

int main(void) {

  i2c_init();
  motor_init();
  mux_init_sensors();
  hc_sr_init();
  lsm303agr_init();
  // nrf_802154_configure(true);

  auto_drive();
  nrf_delay_ms(500);
  replay_path();

  while (1) {
    //get_heading();
    // float dist = find_distance();
    // printf("Cur dist in in: %f\n\n", dist);
    //printf("at end? %i\n", mux_check_end());
    //detect_color(RED, 10);
    nrf_delay_ms(250);
  }
}

