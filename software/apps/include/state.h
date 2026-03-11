#ifndef STATE_H
#define STATE_H

#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>

extern const nrf_twi_mngr_t* state_i2c;

extern bool state_game_over;
extern bool state_backtracking;
extern bool state_backtracking_changed;

typedef enum {
    DECISION_LEFT,
    DECISION_RIGHT,
    DECISION_ERROR,
} junction_decision;

typedef struct decision_node {
    junction_decision decision;
    struct decision_node* prev;
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

typedef enum {
    STATE_AT_JUNCTION,
    STATE_LEFT_TRIGGERED,
    STATE_RIGHT_TRIGGERED,
    STATE_NO_TRIGGERS,
} line_trigger;

extern line_trigger state_line_trigger;
extern bool state_line_changed;

extern uint16_t state_left_avg_white_reading;
extern uint16_t state_right_avg_white_reading;

#endif
