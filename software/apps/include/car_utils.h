#ifndef CAR_UTILS_H
#define CAR_UTILS_H

#define MOTOR_ADDR 0x59

typedef enum {
    MOTOR_LEFT,
    MOTOR_RIGHT,
} CAR_DIRECTION;

// Enable motor on startup
void motor_init();

// Function to drive straight with a given speed
// Accepts values -127, 127 to go forwards or backwards
void drive(int speed);

// Function which attempts to drive on autopilot.
// When it detects it has found a junction it sets a
// state flag as such and blocks waiting for a radio interrupt.
// Takes input as to how fast to drive and how fast to turn
// as well as a threshold for variations in line detection
void auto_drive(int drive_speed, int turn_speed, int threshold);

// Functions to drive given motors with given speeds
// Accepts values -127, 127 to go forwards or backwards
void drive_left(int speed);
void drive_right(int speed);


// Function to make a decision at a junction
// Accepts a direction and a speed -127, 127
void turn(CAR_DIRECTION direction, int speed);

#endif
