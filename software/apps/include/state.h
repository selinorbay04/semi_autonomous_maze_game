#ifndef STATE_H
#define STATE_H

#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>

extern const nrf_twi_mngr_t* state_i2c;
extern const nrf_twi_mngr_t* internal_i2c;

extern volatile bool state_game_over;
extern volatile bool state_backtracking;
extern volatile bool state_backtracking_changed;

extern int state_drive_speed;
extern int state_turn_speed;
extern const int state_line_threshold;
extern const int state_red_threshold;

typedef enum {
    DECISION_LEFT,
    DECISION_RIGHT,
    DECISION_BACKTRACK,
    DECISION_ERROR,
} junction_decision;

typedef struct decision_node {
    junction_decision decision;
    struct decision_node* prev;
    struct decision_node* next;
} decision_node;

extern decision_node* state_decision_stack;

// Stack helper functions, call push with the
// decision that was made, pop returns the
// last decision on the top of the stack and frees it
// peek returns the last decision but does not free it
// Pop and push return DECISION_ERROR upon failure (empty stack)
void push_decision(junction_decision decision);
junction_decision pop_decision();
junction_decision peek_decision();

void print_decision_stack();

typedef enum {
    STATE_AT_JUNCTION,
    STATE_LEFT_TRIGGERED,
    STATE_RIGHT_TRIGGERED,
    STATE_NO_TRIGGERS,
} line_trigger;

extern volatile line_trigger state_line_trigger;
extern volatile bool state_line_changed;

extern uint16_t state_left_avg_white_reading;
extern uint16_t state_right_avg_white_reading;

#endif
