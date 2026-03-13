
#include "nrf.h"
#include "nrf_twi_mngr.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "state.h"

NRF_TWI_MNGR_DEF(i2c_def, 1, 0);
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 1);

const nrf_twi_mngr_t* state_i2c = &i2c_def;
const nrf_twi_mngr_t* internal_i2c = &twi_mngr_instance;

bool volatile state_game_over = false;
bool volatile state_backtracking = false;
bool volatile state_backtracking_changed = false;

int state_drive_speed = 40;
int state_turn_speed = 25;
const int state_line_threshold = 50;

line_trigger volatile state_line_trigger = STATE_NO_TRIGGERS;
bool volatile state_line_changed = false;

uint16_t state_left_avg_white_reading = 0;
uint16_t state_right_avg_white_reading = 0;

decision_node* state_decision_stack = NULL;

void push_decision(junction_decision decision) {
    decision_node* new = (decision_node*)malloc(sizeof(decision_node));

    if (new == NULL) {
        printf("ERROR: failed to allocate a new decision node\n");
    } else {
        new->decision = decision;
        new->prev = state_decision_stack;
        state_decision_stack = new;
    }
}

junction_decision pop_decision() {
    junction_decision ret_val;

    if (state_decision_stack == NULL) {
        printf("ERROR: attempting to pop from empty stack\n");
        return DECISION_ERROR;
    } else {
        ret_val = state_decision_stack->decision;
        decision_node* free_node = state_decision_stack;
        state_decision_stack = state_decision_stack->prev;
        free(free_node);
    }

    return ret_val;
}

junction_decision peek_decision() {
    if (state_decision_stack == NULL) {
        printf("ERROR: attempting to peek at empty stack\n");
        return DECISION_ERROR;
    } else {
        return state_decision_stack->decision;
    }
}

