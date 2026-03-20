
#include "hc-sr04.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "car_utils.h"
#include "i2c_utils.h"
#include "pca9548a.h"
#include "state.h"
#include "radio_utils.h"

// Packages to send with motor commands
uint8_t en_data[2] = {0x70, 1};
uint8_t motor1[2] = {0x20, 129};
uint8_t motor2[2] = {0x21, 129};



static volatile uint8_t pending_cmd   = 0;
static volatile bool    cmd_pending   = false;
static volatile bool    resignal_receive = false;


void nrf_802154_received_raw(uint8_t* p_data, int8_t power, uint8_t lqi) {
    pending_cmd   = recieve_pkt_raw(p_data); 
    cmd_pending   = true;
    resignal_receive = true;
    printf("Recieved command: %x\n", pending_cmd);
}


// const nrf_twi_mngr_transfer_t en_motor = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, en_data, 2, 0);
// const nrf_twi_mngr_transfer_t start_motor1 = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor1, 2, 0);
// const nrf_twi_mngr_transfer_t start_motor2 = NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor2, 2, 0);

// Takes pointer to motor command and a speed to set it to
static void set_speed(uint8_t* motor, int speed){
    // Convert int speed to motor speed
    // 127 is "zero" for motor
    uint8_t motor_speed = 127;

    // Set ceiling and floor
    if (speed > 128) {
        motor_speed = 255;
    } else if (speed < -127) {
        motor_speed = 0;
    } else {
        motor_speed += speed;
    }

    // Set motor to motor speed
    motor[1] = motor_speed;
}

void motor_init() {
    i2c_write_packet(MOTOR_ADDR, en_data, 2, 0);
    drive(0);
    nrf_802154_configure(false);
    recieve_();
}

void drive(int speed) {
    set_speed(motor1, speed);
    set_speed(motor2, speed);
    i2c_write_packet(MOTOR_ADDR, motor1, 2, NRF_TWI_MNGR_NO_STOP);
    i2c_write_packet(MOTOR_ADDR, motor2, 2, 0);
}

void drive_right(int speed) {
    set_speed(motor1, speed);
    i2c_write_packet(MOTOR_ADDR, motor1, 2, 0);
}

void drive_left(int speed) {
    set_speed(motor2, speed);
    i2c_write_packet(MOTOR_ADDR, motor2, 2, 0);
}

void turn_in_place(CAR_DIRECTION direction) {
    switch (direction) {
        case MOTOR_LEFT:
            drive_left(-state_turn_speed);
            drive_right(state_turn_speed);
            break;
        case MOTOR_RIGHT:
            drive_left(state_turn_speed);
            drive_right(-state_turn_speed);
            break;
    }
}

void auto_drive() {

    drive(state_drive_speed);

    while (!state_game_over) {

        mux_update_line_state();
        // Check distance to barrier,
        // if close enough we should set state_backtracking
        // and invert drive_speed and turn_speed until we
        // hit a junction. Then invert them again and pop_decision
        // and push the opposite decision
        if (check_hit_wall()) {
            printf("HIT WALL: BACKTRACK\n");
            push_decision(DECISION_BACKTRACK);
            state_line_changed = true;
            turn_around();
        }

        if (state_line_changed) {
            print_decision_stack();
            switch (state_line_trigger) {
                case STATE_NO_TRIGGERS:
                    drive(state_drive_speed);
                    state_line_changed = false;
                    break;
                case STATE_LEFT_TRIGGERED:
                    turn_in_place(MOTOR_LEFT);
                    state_line_changed = false;
                    break;
                case STATE_RIGHT_TRIGGERED:
                    turn_in_place(MOTOR_RIGHT);
                    state_line_changed = false;
                    break;
                case STATE_AT_JUNCTION:
                    // Send radio message and wait
                    // for radio interrupt
                    drive(0);
                    state_line_changed = false;

                    send_radio_status(AT_JUNCTION);

                    while (pending_cmd == 0) {
                        // spin waiting for command
                    }

                    if (pending_cmd == TURN_RIGHT)
                        take_right();
                    if (pending_cmd == TURN_LEFT)
                        take_left();

                    pending_cmd = 0;
                    break;
            }
        }

        state_game_over = mux_check_end();
    }

    drive(0);
    printf("Reached the end!\n");
    turn_around();
}

void substitute_backtrack(decision_node* backtrack_node, uint8_t depth) {
    print_decision_stack();
    decision_node* previous_node = backtrack_node;
    decision_node* next_node = backtrack_node;

    for (int i = 0; i < depth; i++) {
        if (previous_node->prev == NULL || next_node->next == NULL) {
            printf("ERROR: attempting to sub with invalid stack\n");
            state_decision_stack = NULL;
            return;
        }
        previous_node = previous_node->prev;
        next_node = next_node->next;
    }

    // If you find equal decisions, do the sub
    if (previous_node->decision == next_node->decision) {
        decision_node* new_decision = (decision_node*)malloc(sizeof(decision_node));
        if (new_decision == NULL) {
            printf("ERROR: failed to malloc new decision during sub\n");
            return;
        }

        if (previous_node->decision == DECISION_LEFT) {
            new_decision->decision = DECISION_RIGHT;
        } else {
            new_decision->decision = DECISION_LEFT;
        }

        if (previous_node->prev != NULL) {
            previous_node->prev->next = new_decision;
            new_decision->prev = previous_node->prev;
        } else {
            new_decision->prev = NULL;
        }

        if (next_node->next != NULL) {
            next_node->next->prev = new_decision;
            new_decision->next = next_node->next;
        } else {
            // Top of stack needs to be reset
            state_decision_stack = new_decision;
        }

        // Free the unused nodes just for fun :P
        while (previous_node != backtrack_node) {
            decision_node* temp = previous_node;
            previous_node = previous_node->next;
            free(temp);
        }
        while (next_node != backtrack_node) {
            decision_node* temp = next_node;
            next_node = next_node->prev;
            free(temp);
        }
        // Free the backtrack node
        free(next_node);
    } else {
        // Otherwise keep looking
        substitute_backtrack(backtrack_node, depth + 1);
    }
    print_decision_stack();
}

decision_node* traverse_for_backtrack() {

    decision_node* current = state_decision_stack;

    while(current != NULL){

        if(current->decision == DECISION_BACKTRACK){
            printf("Backtracking node found\n");
            break;
        }

        current = current->prev;
    }

    return current;
}

void invert_stack_decisions() {
    decision_node* current = state_decision_stack;

    while(current != NULL){

        if(current->decision == DECISION_LEFT){
            current->decision = DECISION_RIGHT;
        } else {
            current->decision = DECISION_LEFT;
        }

        current = current->prev;
    }
}

void replay_path() {

    state_game_over = false;
    state_replaying = true;

    decision_node* backtrack_node;

    while ((bool)(backtrack_node = traverse_for_backtrack())) {
        substitute_backtrack(backtrack_node, 1);
    }

    invert_stack_decisions();

    // Print new stack
    printf("NEW STACK AFTER SUB\n");
    print_decision_stack();

    // Test code to see replay of stack
    junction_decision volatile replay = pop_decision();
    while (replay != DECISION_ERROR && !state_game_over) {

        mux_update_line_state();

        if (state_line_changed) {
            printf("CURRENT PENDING DECISION: %i\n", replay);
            switch (state_line_trigger) {
                case STATE_NO_TRIGGERS:
                    drive(state_drive_speed);
                    state_line_changed = false;
                    break;
                case STATE_LEFT_TRIGGERED:
                    turn_in_place(MOTOR_LEFT);
                    state_line_changed = false;
                    break;
                case STATE_RIGHT_TRIGGERED:
                    turn_in_place(MOTOR_RIGHT);
                    state_line_changed = false;
                    break;
                case STATE_AT_JUNCTION:
                    state_line_changed = false;
                    if (replay == DECISION_RIGHT) {
                        take_right();
                    } else {
                        take_left();
                    }
                    replay = pop_decision();
                    break;
            }
        }

        state_game_over = mux_check_end();
    }
    drive(0);
}

// Turn until you're no longer at a junction, then stop
// and return back to main drive loop
void take_turn(CAR_DIRECTION turn_direction) {
    printf("taking turn\n");

    switch (turn_direction) {
        case MOTOR_LEFT:
            drive(state_drive_speed);
            nrf_delay_ms(250);
            state_turn_speed *= TURN_BOOST;
            turn_in_place(MOTOR_LEFT);
            state_turn_speed /= TURN_BOOST;
            // Attempt to turn past the first black line
            while (state_line_trigger == STATE_RIGHT_TRIGGERED ||
                state_line_trigger == STATE_AT_JUNCTION) {
                mux_update_line_state();

                // If the left is also trig'd, reverse it
                if (state_line_trigger == STATE_AT_JUNCTION) {
                    drive_left(-state_turn_speed);
                } else {
                    drive_left(0);
                }
            }
            // Continue to turn until the next black line
            while (state_line_trigger != STATE_RIGHT_TRIGGERED) {
                mux_update_line_state();

                // If the left is also trig'd, reverse it
                if (state_line_trigger == STATE_LEFT_TRIGGERED) {
                    drive_left(-state_turn_speed);
                } else {
                    drive_left(0);
                }
            }
            //drive(0);
            //nrf_delay_ms(100000);
            break;
        case MOTOR_RIGHT:
            drive(state_drive_speed);
            nrf_delay_ms(250);
            state_turn_speed *= TURN_BOOST;
            turn_in_place(MOTOR_RIGHT);
            state_turn_speed /= TURN_BOOST;
            // Attempt to turn past the first black line
            while (state_line_trigger == STATE_LEFT_TRIGGERED ||
                state_line_trigger == STATE_AT_JUNCTION) {
                mux_update_line_state();

                // If the right is also trig'd, reverse it
                if (state_line_trigger == STATE_AT_JUNCTION) {
                    drive_right(-state_turn_speed);
                } else {
                    drive_right(0);
                }
            }
            // Continue to turn until the next black line
            while (state_line_trigger != STATE_LEFT_TRIGGERED) {
                mux_update_line_state();

                // If the right is also trig'd, reverse it
                if (state_line_trigger == STATE_RIGHT_TRIGGERED) {
                    drive_right(-state_turn_speed);
                } else {
                    drive_right(0);
                }
            }
            //drive(0);
            //nrf_delay_ms(100000);
            break;
    }

    if (!state_replaying) {
        switch (turn_direction) {
            case MOTOR_RIGHT:
                push_decision(DECISION_RIGHT);
                break;
            case MOTOR_LEFT:
                push_decision(DECISION_LEFT);
                break;
        }
    }

    printf("done taking turn\n");
    drive(0);
}


void turn_around() {
    int temp = state_turn_speed;
    state_turn_speed = 35;
    turn_in_place(MOTOR_RIGHT);
    state_turn_speed = temp;
    nrf_delay_ms(1700);
    drive(0);
}
