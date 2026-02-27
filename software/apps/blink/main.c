// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_twi_mngr.h"


// Pin configurations
#include "microbit_v2.h"

NRF_TWI_MNGR_DEF(twi_mngr, 1, 0);


int main(void) {

  // Initialize.
  // nrf_gpio_cfg_output(LED_MIC);

  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;

  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_DRV_TWI_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  nrf_twi_mngr_init(&twi_mngr, &i2c_config);

  // Enter main loop.
  while (1) {
    nrf_gpio_pin_toggle(LED_MIC);
    nrf_delay_ms(500);
  }
}

