#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "microbit_v2.h"
#include "sdk_errors.h"
#include <nrfx_gpiote.h>
#include "nrf_802154.h"
#include "radio_utils.h"


static void BTN_A_handler(void) {
    uint8_t* pkt = load_pkt(TURN_LEFT, 1);
    send_pkt(pkt);
}

static void BTN_B_handler(void) {
    uint8_t* pkt = load_pkt(TURN_RIGHT, 1);
    send_pkt(pkt);
}

void gpio_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t _unused) {
    if (pin == BTN_A) {
        printf("Button A pressed\n");
        BTN_A_handler();
    } else if (pin == BTN_B) {
        printf("Button B pressed\n");
        BTN_B_handler();
    }
}


int main(void) {
    nrf_delay_ms(100);
    printf("I'm running!\n");

    nrf_802154_configure(true);

    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    nrfx_gpiote_in_init(BTN_A, &config, gpio_handler);
    nrfx_gpiote_in_event_enable(BTN_A, true);

    nrfx_gpiote_in_init(BTN_B, &config, gpio_handler);
    nrfx_gpiote_in_event_enable(BTN_B, true);

    while (1) {
        nrf_delay_ms(50);
    }
}
