// Radio 15.4 receive app
//
// Receives wireless packets and drives the Moto:bit via I2C

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"
#include "sdk_errors.h"
#include "nrf_802154.h"
#include "radio_utils.h"
#include "car_utils.h"
#include "state.h"
#include "i2c_utils.h"
#include "hc-sr04.h"

// #define MOTOR_ADDR 0x59

// NRF_TWI_MNGR_DEF(twi_mngr, 1, 0);

// static nrf_drv_twi_config_t i2c_config;

// static uint8_t en_data[2]    = {0x70, 1};
// static uint8_t motor1_data[2] = {0x20, 129}; //stop = 128 ?
// static uint8_t motor2_data[2] = {0x21, 129}; //stop = 128 ?


//Command queue for passing data from the radio receive callback to the main loop.
static volatile uint8_t pending_cmd   = 0;
static volatile bool    cmd_pending   = false;
static volatile bool    resignal_receive = false;


// Called by the 802.15.4 driver from interrupt context when a packet arrives 
// The interrupt comes, saves the command and sets two flags, then exits
void nrf_802154_received_raw(uint8_t* p_data, int8_t power, uint8_t lqi) {
    pending_cmd   = recieve_pkt_raw(p_data); 
    cmd_pending   = true;
    resignal_receive = true;
    printf("Recieved command: %x\n", pending_cmd);
}


static void drive_motors(uint8_t cmd) {
    if (cmd == TURN_LEFT) {
        turn(MOTOR_LEFT, 40);
        printf("Turning left\n");
    } else if (cmd == TURN_RIGHT) {
        turn(MOTOR_RIGHT, 40);
        printf("Turning right\n");
    } else {
        printf("Unknown command: 0x%02X\n", cmd);
        return;
    }

    // nrf_twi_mngr_transfer_t transfers[3] = {
    //     NRF_TWI_MNGR_WRITE(MOTOR_ADDR, en_data,     2, 0),
    //     NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor1_data, 2, 0),
    //     NRF_TWI_MNGR_WRITE(MOTOR_ADDR, motor2_data, 2, 0),
    // };

    // ret_code_t err = nrf_twi_mngr_perform(&twi_mngr, &i2c_config, transfers, 3, NULL);
    // if (err != NRF_SUCCESS) {
    //     printf("I2C perform failed: %d\n", (int)err);
    // }
}


int main(void) {
    nrf_delay_ms(100);
    printf("Receiver running!\n");

    // i2c_config             = (nrf_drv_twi_config_t)NRF_DRV_TWI_DEFAULT_CONFIG;
    // i2c_config.scl         = EDGE_P19;
    // i2c_config.sda         = EDGE_P20;
    // i2c_config.frequency   = NRF_DRV_TWI_FREQ_100K;
    // i2c_config.interrupt_priority = 0;

    // ret_code_t err = nrf_twi_mngr_init(&twi_mngr, &i2c_config);
    // if (err != NRF_SUCCESS) {
    //     printf("I2C init failed: %d\n", (int)err);
    // } else {
    //     printf("I2C init OK\n");
    // }

    i2c_init();
    motor_init();
    hc_sr_init();

    // Configure radio as receiver and listen 
    nrf_802154_configure(false);
    recieve_();

    static bool wall_reported = false;
    static bool end_reported  = false;

    while (true) {
        if (cmd_pending) {
            printf("attempting to handle pending command\n");
            cmd_pending = false;
            drive_motors(pending_cmd);
        }
        if (resignal_receive) {
            resignal_receive = false;
            recieve_();
        }

        // Junction: state_line_trigger is set by the line sensor 
        if (state_line_changed && state_line_trigger == STATE_AT_JUNCTION) {
            state_line_changed = false;// reset so it can fire again next time
            send_radio_status(AT_JUNCTION);
        }

        // Wall: check_hit_wall() uses the ultrasonic sensor
        if (check_hit_wall() && !wall_reported) {
            wall_reported = true;
            send_radio_status(AT_WALL);
        } else if (!check_hit_wall()) {
            wall_reported = false; // reset so it can fire again next time
        }

        // End: state_game_over is set when the maze is solved
        if (state_game_over && !end_reported) {
            end_reported = true;
            send_radio_status(MAZE_END);
        }
    }
}
