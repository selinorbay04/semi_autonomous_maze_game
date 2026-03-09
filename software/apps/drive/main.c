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
#include "state.h"
#include "car_utils.h"

// Pin configurations
#include "microbit_v2.h"

int main(void) {

  i2c_init();

  motor_init();
  //drive(15);
  drive_left(-55);
  drive_right(55);

  // Enter main loop.
  while (1) {
    nrf_delay_ms(50);

    // nrf_twi_mngr_perform(&twi_mngr, &i2c_config, transfer_array, 3, NULL);
  }
}

