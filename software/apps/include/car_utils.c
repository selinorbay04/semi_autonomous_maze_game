
#include "nrf.h"
#include "nrf_twi_mngr.h"
#include <stdint.h>
#include <stdio.h>

#include "car_utils.h"
#include "i2c_utils.h"
#include "pca9548a.h"
#include "state.h"

// Packages to send with motor commands
uint8_t en_data[2] = {0x70, 1};
uint8_t motor1[2] = {0x20, 129};
uint8_t motor2[2] = {0x21, 129};

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

void turn(CAR_DIRECTION direction, int speed) {
    switch (direction) {
        case LEFT:
            drive_left(-speed);
            drive_right(speed);
            break;
        case RIGHT:
            drive_left(speed);
            drive_right(-speed);
            break;
    }
}

void auto_drive(int drive_speed, int turn_speed, int threshold) {

    drive(drive_speed);

    while (!state_game_over) {

        mux_update_line_state(threshold);

        if (state_line_changed) {
            switch (state_line_trigger) {
                case STATE_NO_TRIGGERS:
                    drive(drive_speed);
                    state_line_changed = false;
                    break;
                case STATE_LEFT_TRIGGERED:
                    turn(LEFT, turn_speed);
                    state_line_changed = false;
                    break;
                case STATE_RIGHT_TRIGGERED:
                    turn(RIGHT, turn_speed);
                    state_line_changed = false;
                    break;
                case STATE_AT_JUNCTION:
                    // Send radio message and wait
                    // for radio interrupt
                    // uint8_t* pkt = load_pkt(AT_JUNCTION, 1);
                    // send_pkt(pkt);
                    // Some sort of sleep or blocking
                    drive(0);
                    break;
            }
        }

    }
}

