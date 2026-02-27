// Blink app
//
// Blinks an LED

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "nrfx_twi.h"
#include "nrf_drv_twi.h"


// Pin configurations
#include "microbit_v2.h"

#define NRFX_TWI0_INST_IDX 0

static const nrfx_twi_t my_twi = NRFX_TWI0_INST_IDX(0);

nrfx_twi_config_t twi_config = {
  .scl = NRF_GPIO_PIN_MAP(0, 19),
  .sda = NRF_GPIO_PIN_MAP(0, 20),
  .frequency = NRF_TWI_FREQ_100K,
  .interrupt_priority = APP_IRQ_PRIORITY_HIGH
};

nrfx_twi_evt_handler_t twi_event_handler = NULL;

void twi_init(void) {
  ret_code_t err_code = nrfx_twi_init(&my_twi, &twi_config, twi_event_handler, NULL);
  APP_ERROR_CHECK(err_code);
  nrfx_twi_enable(&my_twi);
}






int main(void) {

  // Initialize.
  nrf_gpio_cfg_output(LED_MIC);

  twi_init();

  


  // Enter main loop.
  while (1) {
    nrf_gpio_pin_toggle(LED_MIC);
    nrf_delay_ms(500);
  }
}

