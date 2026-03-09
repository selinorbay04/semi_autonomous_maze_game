// LED Matrix app
//
// Display messages on the LED matrix

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"

#include "led_matrix.h"
#include "microbit_v2.h"
#include "snake_game.h"

int main(void) {
  printf("Board started!\n");
  
  // initialize LED matrix driver
  led_matrix_init();
  snake_game_init();


  // call other functions here
  // char input[] = "Hi CE346!";
  // char input2[] = "It works!";
  // set_string(input);
  // loop forever
  while (1) {
    nrf_delay_ms(500);
    snake_game_advance_state();
    // set_string(input2);
  }
}

