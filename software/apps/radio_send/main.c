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
#include "led_matrix.h"

//If set, cycles through all LED states using buttons 
#define STANDALONE_TEST 0

static volatile bool resignal_receive = false;

// Pre-declared frames from led_matrix.h macros
static bool left_arrow[5][5]  = LEFT_ARROW;
static bool right_arrow[5][5] = RIGHT_ARROW;
static bool q_state[5][5]     = Q_STATE;
static bool x_state[5][5]     = X_STATE;
static bool smiley[5][5]      = SMILEY_STATE;

// Called when a status packet arrives from the robot
static void update_display(uint8_t status) {
    switch (status) {
        case AT_JUNCTION:
            led_matrix_blink_stop();
            set_state(q_state);
            printf("Junction detected — Q_STATE\n");
            break;
        case AT_WALL:
            led_matrix_blink_start(x_state);
            printf("Wall detected — X_STATE blinking\n");
            break;
        case MAZE_END:
            led_matrix_blink_start(smiley);
            printf("Maze end — SMILEY blinking\n");
            break;
        default:
            break; 
    }
}

// Received callback — fires when any packet arrives at the controller
void nrf_802154_received_raw(uint8_t* p_data, int8_t power, uint8_t lqi) {
    uint8_t status = recieve_pkt_raw(p_data);
    update_display(status);
    resignal_receive = true;
}

#if STANDALONE_TEST
// Test mode: A cycles through all states, B resets
static uint8_t test_idx = 0;

static void BTN_A_handler(void) {
    test_idx = (test_idx + 1) % 5;
    led_matrix_blink_stop();
    switch (test_idx) {
        case 0: { bool empty[5][5] = {false}; set_state(empty); }    break;
        case 1: set_state(left_arrow);              break;
        case 2: set_state(q_state);                 break;
        case 3: led_matrix_blink_start(x_state);    break;
        case 4: led_matrix_blink_start(smiley);     break;
    }
}

static void BTN_B_handler(void) {
    test_idx = 1;
    led_matrix_blink_stop();
    set_state(right_arrow);
}

#else
//show arrow + send radio command

static void BTN_A_handler(void) {
    led_matrix_blink_stop();
    set_state(left_arrow);
    send_radio_command(TURN_LEFT);
}

static void BTN_B_handler(void) {
    led_matrix_blink_stop();
    set_state(right_arrow);
    send_radio_command(TURN_RIGHT);
}


#endif

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
    printf("Controller running!\n");

    led_matrix_init();
    nrf_802154_configure(true);
    recieve_(); // listen for incoming status from robot

    nrfx_gpiote_init();
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    nrfx_gpiote_in_init(BTN_A, &config, gpio_handler);
    nrfx_gpiote_in_event_enable(BTN_A, true);

    nrfx_gpiote_in_init(BTN_B, &config, gpio_handler);
    nrfx_gpiote_in_event_enable(BTN_B, true);

    while (1) {
        if (resignal_receive) {
            resignal_receive = false;
            recieve_();
        }
        nrf_delay_ms(50);
    }
}
