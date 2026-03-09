// LED Matrix Driver
// Displays characters on the LED matrix

#include <stdbool.h>
#include <stdio.h>

#include "nrf_gpio.h"
#include "app_timer.h"

#include "led_matrix.h"
#include "font.h"
#include "microbit_v2.h"

APP_TIMER_DEF(led_timer);
APP_TIMER_DEF(string_timer);

//bool led_state = false;

uint32_t row_pins[] = {LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5};
uint32_t col_pins[] = {LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5};

char* message = "";
int curr_char = 0;

bool led_states[5][5] = {false}; 
uint16_t curr_row = 0;

void read_char(char c) {
  // read character from font and update led_states
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      led_states[row][col] = (font[c][row] >> col) & 1;
    }
  }
}

void set_string(char* str) {
  // read each character in the string and update led_states
  // for (int i = 0; str[i] != 0; i++) {
  //   read_char(str[i]);
  // }
  message = str;
  curr_char = 0;
}

static void led_timer_handler(void* _unused) {
  // toggle LEDs here
  nrf_gpio_pin_clear(row_pins[curr_row]);
  curr_row += 1;
  if (curr_row >= 5) {
    curr_row = 0;
  }
  nrf_gpio_pin_set(row_pins[curr_row]);

  for (int col = 0; col < 5; col++) {
    if (led_states[curr_row][col]) {
      nrf_gpio_pin_clear(col_pins[col]);
    } else {
      nrf_gpio_pin_set(col_pins[col]);
    }
  }
}

static void increment_string_timer(void* unused){
  // increment string timer here
  
  if (message[curr_char] == 0) {
    curr_char = 0;
  }
  read_char(message[curr_char]);
  curr_char += 1;
}

void set_state(bool states[5][5]) {
  // set the led_states to the provided states
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      led_states[row][col] = states[row][col];
    }
  }
}

void led_matrix_init(void) {
  // initialize row pins
  nrf_gpio_pin_dir_set(LED_ROW1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_ROW5, NRF_GPIO_PIN_DIR_OUTPUT);

  // initialize col pins
  nrf_gpio_pin_dir_set(LED_COL1, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL2, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL3, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL4, NRF_GPIO_PIN_DIR_OUTPUT);
  nrf_gpio_pin_dir_set(LED_COL5, NRF_GPIO_PIN_DIR_OUTPUT);
  // set default values for pins


  // nrf_gpio_pin_set(row_pins[0]);
  // nrf_gpio_pin_set(col_pins[4]);

  // initialize timer(s) (Step 2 and onwards)
  app_timer_init();
  
  // app_timer_create(&string_timer, APP_TIMER_MODE_REPEATED, increment_string_timer);
  // app_timer_start(string_timer, 32768, NULL);

  app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, led_timer_handler);
  app_timer_start(led_timer, 100, NULL);
  // set default state for the LED display (Step 3 and onwards)

}


