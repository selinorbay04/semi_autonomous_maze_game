// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nrf_twi_mngr.h"


// Pin configurations
#include "microbit_v2.h"
#include "sdk_errors.h"

#define MOTOR_ADDR 0x59
NRF_TWI_MNGR_DEF(twi_mngr, 1, 0);

//uint8_t transfer_data[6] = {0x70, 1, 0x21, 255, 0x20, 255};
uint8_t en_data[2] = {0x70, 1};
uint8_t motor1[2] = {0x20, 129};
uint8_t motor2[2] = {0x21, 129};

const nrf_twi_mngr_transfer_t en_motor = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, en_data, 2, 0);
const nrf_twi_mngr_transfer_t start_motor1 = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor1, 2, 0);
const nrf_twi_mngr_transfer_t start_motor2 = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor2, 2, 0);

nrf_twi_mngr_transfer_t transfer_array[3] = {
  en_motor,
  start_motor1,
  start_motor2
};

// nrf_twi_mngr_transfer_t transfer = {
//   transfer_data,
//   2,
//   MOTOR_ADDR,
//   0
// };


int main(void) {

  // Initialize.
  // nrf_gpio_cfg_output(LED_MIC);
  nrf_delay_ms(100);

  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  printf("I'm running!\n");
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_DRV_TWI_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  ret_code_t init_ret = nrf_twi_mngr_init(&twi_mngr, &i2c_config);
  if (init_ret != NRF_SUCCESS) {
    printf("some error in init twi\n");
  } else {
    printf("successful twi init\n");
  }

  ret_code_t perform_twi = nrf_twi_mngr_perform(&twi_mngr, &i2c_config, transfer_array, 3, NULL);
  if (perform_twi != NRF_SUCCESS) {
    printf("some error in perform twi\n");
  } else {
    printf("successful twi perform\n");
  }
  if (perform_twi == NRF_ERROR_BUSY) {
    printf("error was that bus was busy\n");
  }

  bool ascending = false;

  // Enter main loop.
  while (1) {
    nrf_delay_ms(50);

    if (motor1[1] == 177) {
      ascending = false;
    } else if (motor1[1] == 77) {
      ascending = true;
    }

    if (ascending) {
      motor1[1] += 1;
      motor2[1] += 1;
    } else {
      motor1[1] -= 1;
      motor2[1] -= 1;
    }

    nrf_twi_mngr_perform(&twi_mngr, &i2c_config, transfer_array, 3, NULL);
  }
}

