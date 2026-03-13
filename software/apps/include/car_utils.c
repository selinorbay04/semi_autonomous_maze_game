
#include "hc-sr04.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include <stdint.h>
#include <stdio.h>

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


nrf_802154_configure(False);
recieve_();

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

        mux_update_line_state(state_line_threshold);
        // Check distance to barrier,
        // if close enough we should set state_backtracking
        // and invert drive_speed and turn_speed until we
        // hit a junction. Then invert them again and pop_decision
        // and push the opposite decision
        if (check_hit_wall() && !state_backtracking_changed) {
            printf("HIT WALL: BACKTRACK\n");
            state_backtracking = true;
            state_backtracking_changed = true;
            state_drive_speed = -state_drive_speed;
            //turn_speed = -turn_speed;
        }

        if (state_line_changed || state_backtracking_changed) {
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
                    // uint8_t* pkt = load_pkt(AT_JUNCTION, 1);
                    // send_pkt(pkt);
                    // Some sort of sleep or blocking

                    // Test code to see replay of stack
                    drive(0);
                    
                    send_radio_status(AT_JUNCTION);

                    if(pending_cmd = TURN_RIGHT) take_right(turn_speed, threshold); 
                    if(pending_cmd = TURN_LEFT) take_left(turn_speed, threshold); 
                    // push_decision(DECISION_RIGHT);
                    //take_right(turn_speed, threshold);
                    // nrf_delay_ms(4000);
                    //state_game_over = true;
                    break;
            }
        }
    }
}

void replay_path(int drive_speed, int turn_speed, int threshold) {
    // Test code to see replay of stack
    junction_decision replay = pop_decision();
    while (replay != DECISION_ERROR) {
        if (replay == DECISION_LEFT) {
            turn_in_place(MOTOR_RIGHT);
        } else if (replay == DECISION_RIGHT) {
            turn_in_place(MOTOR_LEFT);
        }
        nrf_delay_ms(500);
        drive(-drive_speed);
        nrf_delay_ms(500);
        replay = pop_decision();
    }
    drive(0);
}

// Turn until you're no longer at a junction, then stop
// and return back to main drive loop
void take_turn(CAR_DIRECTION turn_direction) {
    printf("taking turn\n");

    switch (turn_direction) {
        case MOTOR_LEFT:
            drive_right(state_turn_speed * TURN_BOOST);
            drive_left(-10);
            while (state_line_trigger != STATE_RIGHT_TRIGGERED) {
                mux_update_line_state(state_line_threshold);
            }
            turn_in_place(MOTOR_RIGHT);
            while (state_line_trigger == STATE_RIGHT_TRIGGERED) {
                mux_update_line_state(state_line_threshold);
            }
            break;
        case MOTOR_RIGHT:
            drive(state_turn_speed);
            nrf_delay_ms(250);
            state_turn_speed *= TURN_BOOST;
            turn_in_place(MOTOR_RIGHT);
            state_turn_speed /= TURN_BOOST;
            // Attempt to turn past the first black line
            while (state_line_trigger == STATE_LEFT_TRIGGERED ||
                state_line_trigger == STATE_AT_JUNCTION) {
                mux_update_line_state(state_line_threshold);

                // If the right is also trig'd, reverse it
                if (state_line_trigger == STATE_AT_JUNCTION) {
                    drive_right(-state_turn_speed);
                } else {
                    drive_right(0);
                }
            }
            // Continue to turn until the next black line
            while (state_line_trigger != STATE_LEFT_TRIGGERED) {
                mux_update_line_state(state_line_threshold);

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

    if (state_backtracking) {
        state_backtracking = false;
        state_backtracking_changed = true;
        state_drive_speed = -state_drive_speed;
    }

    printf("done taking turn\n");
    drive(0);
}

