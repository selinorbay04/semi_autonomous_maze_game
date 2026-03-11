

#include "hc-sr04.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_timer.h"
#include "nrfx_gpiote.h"
#include "nrfx_timer.h"
#include <stdbool.h>
#include <stdint.h>

static nrfx_timer_t TIMER4 = NRFX_TIMER_INSTANCE(0);
static bool echo_received = false;
static uint32_t cc_read = 0;

static void echo_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    nrfx_gpiote_in_event_disable(ECHO_PIN);
    echo_received = true;

    // When you get the echo, pause and read timer
    nrfx_timer_pause(&TIMER4);
    cc_read = nrfx_timer_capture(&TIMER4, NRF_TIMER_CC_CHANNEL0);

    nrfx_gpiote_in_event_enable(ECHO_PIN, true);
}

void hc_sr_init() {
    // Set up one GPIO in and out
    nrf_gpio_pin_dir_set(TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_dir_set(ECHO_PIN, NRF_GPIO_PIN_DIR_INPUT);
    nrf_gpio_pin_clear(TRIG_PIN);
    nrf_gpio_pin_clear(ECHO_PIN);

    // Set an interrupt for the echo pin
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    nrfx_gpiote_in_init(ECHO_PIN, &in_config, echo_handler);
    nrfx_gpiote_in_event_enable(ECHO_PIN, true);

    // Set up a timer to measure time between
    // trig and echo
    nrfx_timer_config_t timer_config = {
        .frequency = NRF_TIMER_FREQ_1MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_32,
        .interrupt_priority = 4,
        .p_context = NULL
    };
    nrfx_timer_init(&TIMER4, &timer_config, NULL);
    nrfx_timer_enable(&TIMER4);
    nrfx_timer_pause(&TIMER4);
}

float find_distance() {
    // Clear and start timer and wait for echo
    echo_received = false;
    nrfx_timer_clear(&TIMER4);
    nrfx_timer_resume(&TIMER4);

    // Drive trigger
    nrf_gpio_pin_set(TRIG_PIN);
    nrf_delay_us(10);
    nrf_gpio_pin_clear(TRIG_PIN);

    // Wait for interrupt from echo
    while (!echo_received) {
        // Spin
    }

    float distance_in = (float)cc_read / 148.0;

    return distance_in;
}

bool check_hit_wall() {
    return true;
}
