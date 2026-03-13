#ifndef CAR_UTILS_H
#define CAR_UTILS_H

#define MOTOR_ADDR 0x59

// Multiplier on whichever motor
// is attempting the turn, trying
// to avoid jittering
#define STABILIZE_BOOST 2
// Multiplier to take turns completely
#define TURN_BOOST 2

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

// Called to make the turn following a decision
// at a junction
#define take_left(turn_speed, threshold) \
    take_turn(turn_speed, MOTOR_LEFT, threshold)
#define take_right(turn_speed, threshold) \
    take_turn(turn_speed, MOTOR_RIGHT, threshold)

    
void take_turn(int turn_speed, CAR_DIRECTION turn_direction, int threshold);

// Functions to drive given motors with given speeds
// Accepts values -127, 127 to go forwards or backwards
void drive_left(int speed);
void drive_right(int speed);

#endif
