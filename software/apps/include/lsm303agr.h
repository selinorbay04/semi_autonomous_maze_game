#ifndef LSM303AGR_H
#define LSM303AGR_H

#include <stdint.h>

// Chip addresses for accelerometer and magnetometer
static const uint8_t LSM303AGR_MAG_ADDRESS = 0x1E;

// Measurement data type
typedef struct {
  float x_axis;
  float y_axis;
  float z_axis;
} lsm303agr_measurement_t;

// Register definitions for magnetometer
typedef enum {
  OFFSET_X_REG_L_M = 0x45,
  OFFSET_X_REG_H_M = 0x46,
  OFFSET_Y_REG_L_M = 0x47,
  OFFSET_Y_REG_H_M = 0x48,
  OFFSET_Z_REG_L_M = 0x49,
  OFFSET_Z_REG_H_M = 0x4A,
  WHO_AM_I_M = 0x4F,
  CFG_REG_A_M = 0x60,
  CFG_REG_B_M = 0x61,
  CFG_REG_C_M = 0x62,
  INT_CTRL_REG_M = 0x63,
  INT_SOURCE_REG_M = 0x64,
  INT_THS_L_REG_M = 0x65,
  INT_THS_H_REG_M = 0x66,
  STATUS_REG_M = 0x67,
  OUTX_L_REG_M = 0x68,
  OUTX_H_REG_M = 0x69,
  OUTY_L_REG_M = 0x6A,
  OUTY_H_REG_M = 0x6B,
  OUTZ_L_REG_M = 0x6C,
  OUTZ_H_REG_M = 0x6D,
} lsm303agr_mag_reg_t;

void lsm303agr_init();

// Return heading in degrees
float get_heading();

#endif
