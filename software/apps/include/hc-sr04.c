

#include "hc-sr04.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_timer.h"
#include "nrfx_gpiote.h"
#include "nrfx_timer.h"
#include <stdbool.h>
#include <stdint.h>

static const nrfx_timer_t TIMER = NRFX_TIMER_INSTANCE(0);
static volatile bool echo_received = false;
static volatile bool echo_started = false;
static volatile uint32_t cc_read = 0;

static void echo_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {

    bool level = nrf_gpio_pin_read(ECHO_PIN);
    // printf("echo trig with level %i\n", level);

    // If lotohi, start timer
    if (level) {
        //printf("lotohi\n");
        if (!echo_started) {
            echo_started = true;
            echo_received = false;
            nrfx_timer_clear(&TIMER);
            nrfx_timer_enable(&TIMER);
        }
        // if hitolo, stop timer
    } else {
        //printf("hitolo\n");
        if (echo_started) {
            echo_started = false;
            echo_received = true;
            // When you get the echo, pause and read timer
            cc_read = nrfx_timer_capture(&TIMER, NRF_TIMER_CC_CHANNEL0);
            nrfx_timer_disable(&TIMER);
        }
    }

}

static void timer_handler(nrf_timer_event_t event, void* context) {}

void hc_sr_init() {
    // Set up one GPIO in and out
    nrf_gpio_pin_dir_set(TRIG_PIN, NRF_GPIO_PIN_DIR_OUTPUT);
    nrf_gpio_pin_clear(TRIG_PIN);

    // Set an interrupt for the echo pin
    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLDOWN;
    nrfx_gpiote_in_init(ECHO_PIN, &in_config, echo_handler);
    nrfx_gpiote_in_event_enable(ECHO_PIN, true);

    // Set up a timer to measure time between
    // trig and echo
    nrfx_timer_config_t timer_config = NRFX_TIMER_DEFAULT_CONFIG;
    timer_config.frequency = NRF_TIMER_FREQ_1MHz;
    timer_config.bit_width = NRF_TIMER_BIT_WIDTH_32;
    nrfx_timer_init(&TIMER, &timer_config, timer_handler);
    //nrfx_timer_enable(&TIMER);
    nrfx_timer_pause(&TIMER);
    nrfx_timer_clear(&TIMER);
}

float find_distance() {
    // Clear and start timer and wait for echo
    echo_received = false;
    echo_started = false;

    while (nrf_gpio_pin_read(ECHO_PIN)) {
        // Spin
    }

    // Drive trigger
    nrf_gpio_pin_set(TRIG_PIN);
    nrf_delay_us(10);
    nrf_gpio_pin_clear(TRIG_PIN);

    // Wait for interrupt from echo
    while (!echo_received) {
        // Spin
        // printf("spinning...\n");
        // nrf_delay_ms(500);
    }

    //printf("Cur CC read: %i\n", cc_read);
    float distance_in = (float)cc_read / 148.0;

    return distance_in;
}

bool check_hit_wall() {
    return true;
}
