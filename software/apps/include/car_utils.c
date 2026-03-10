
#include "nrf.h"
#include "i2c_utils.h"
#include "nrf_twi_mngr.h"
#include "state.h"
#include <stdint.h>

#include "car_utils.h"

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
