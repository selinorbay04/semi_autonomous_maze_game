#pragma once

#include "nrf.h"
#include "nrf_error.h"
#include "nrf_802154.h"
#include <stdbool.h>
#include <stdint.h>

#define BTN_A 14
#define BTN_B 23

#define TURN_LEFT  0xFF
#define TURN_RIGHT 0x00

#define PSDU_MAX_SIZE (127)
#define FCS_LENGTH    (2)

/* Example flow when Button A pressed:
- GPIO interrupt fires on Micro:bit #1
- gpio_handler builds packet with payload 0xFF (TURN_LEFT)
- 802.15.4 radio transmits packet over the air
- Micro:bit #2 radio receives packet
- nrf_802154_received_raw extracts 0xFF, sets flags
- main loop sees flag, calls drive_motors(0xFF)
- I2C writes sent to Moto:bit at 0x59
- Left motor reverses, right motor goes forward - robot turns left */

// Radio init: pass true for sender, false for receiver
void nrf_802154_configure(bool sender);

//TX callbacks — required by the driver
void nrf_802154_transmitted_raw(const uint8_t* p_frame, uint8_t* p_ack, int8_t power, uint8_t lqi);
void nrf_802154_transmit_failed(const uint8_t* p_frame, nrf_802154_tx_error_t error);
void nrf_802154_tx_started(const uint8_t* p_frame);

// TX helpers
uint8_t* load_pkt(uint8_t payload, uint8_t size);
void     send_pkt(uint8_t* pkt);
void     send_radio_command(uint8_t command_byte);

// RX helpers
void    recieve_(void);
uint8_t recieve_pkt_raw(uint8_t* pkt); 