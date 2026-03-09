
#include "nrf.h"

#define MOTOR_ADDR 0x59

typedef enum {
    LEFT,
    RIGHT,
} CAR_DIRECTION;

// Enable motor on startup
void motor_init();

// Function to drive straight with a given speed
// Accepts values -127, 127 to go forwards or backwards
void drive(int speed);

// Functions to drive given motors with given speeds
// Accepts values -127, 127 to go forwards or backwards
void drive_left(int speed);
void drive_right(int speed);


// Function to make a decision at a junction
// Accepts a direction and a speed -127, 127
void turn(CAR_DIRECTION direction, int speed);
