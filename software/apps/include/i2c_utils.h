#ifndef I2C_UTILS_H
#define I2C_UTILS_H

#include <stdbool.h>
#include <stdint.h>

void i2c_init();

// External functions
void i2c_write_reg(uint8_t i2c_addr,
                  uint8_t reg_addr,
                  uint8_t* buffer,
                  uint8_t length);

void i2c_read_reg(uint8_t i2c_addr,
                  uint8_t reg_addr,
                  uint8_t* buffer,
                  uint8_t length);

#define i2c_write_byte(i2c_addr, buffer, flags) \
    i2c_write_packet(i2c_addr, buffer, 1, flags)

#define i2c_read_byte(i2c_addr, buffer, flags) \
    i2c_read_packet(i2c_addr, buffer, 1, flags)

#define i2c_write_packet(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, false, false)

#define i2c_read_packet(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, true, false)

// Internal functions
void i2c_write_reg_internal(uint8_t i2c_addr,
                  uint8_t reg_addr,
                  uint8_t* buffer,
                  uint8_t length);

void i2c_read_reg_internal(uint8_t i2c_addr,
                  uint8_t reg_addr,
                  uint8_t* buffer,
                  uint8_t length);

#define i2c_write_byte_internal(i2c_addr, buffer, flags) \
    i2c_write_packet_internal(i2c_addr, buffer, 1, flags)

#define i2c_read_byte_internal(i2c_addr, buffer, flags) \
    i2c_read_packet_internal(i2c_addr, buffer, 1, flags)

#define i2c_write_packet_internal(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, false, true)

#define i2c_read_packet_internal(i2c_addr, buffer, length, flags) \
    i2c_manage_packet(i2c_addr, buffer, length, flags, true, true)

void i2c_manage_packet(uint8_t i2c_addr,
                       uint8_t* buffer,
                       uint8_t length,
                       uint8_t flags,
                       bool rx,
                       bool internal);

#endif
