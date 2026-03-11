#ifndef STATE_H
#define STATE_H

#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>

extern const nrf_twi_mngr_t* state_i2c;

extern bool state_game_over;

typedef enum {
    LEFT,
    RIGHT
} junction_decision;

typedef struct decision_node {
    junction_decision decision;
    struct decision_node* prev;
} decision_node;

extern decision_node* state_decision_stack;

void push_decision(junction_decision decision);
junction_decision pop_decision();

typedef enum {
    STATE_AT_JUNCTION,
    STATE_LEFT_TRIGGERED,
    STATE_RIGHT_TRIGGERED,
    STATE_NO_TRIGGERS,
} line_trigger;

extern line_trigger state_line_trigger;
extern bool state_line_changed;

extern uint16_t state_avg_white_reading;

#endif
