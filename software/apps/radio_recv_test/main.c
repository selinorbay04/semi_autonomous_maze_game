

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_802154.h"
#include "microbit_v2.h"
#include "radio_utils.h"

static volatile uint8_t pending_cmd      = 0;
static volatile bool    cmd_pending      = false;
static volatile bool    resignal_receive = false;

void nrf_802154_received_raw(uint8_t* p_data, int8_t power, uint8_t lqi) {
    pending_cmd      = recieve_pkt_raw(p_data);
    cmd_pending      = true;
    resignal_receive = true;
}

static void test_cmd(uint8_t cmd) {
    if (cmd == TURN_LEFT) {
        printf("TEST: received cmd=0x%02X -> TURN_LEFT  (OK)\n", cmd);
    } else if (cmd == TURN_RIGHT) {
        printf("TEST: received cmd=0x%02X -> TURN_RIGHT (OK)\n", cmd);
    } else {
        printf("TEST: received cmd=0x%02X -> UNKNOWN    (unexpected)\n", cmd);
    }
}

int main(void) {
    nrf_delay_ms(100);
    printf("Radio receive test running!\n");
    printf("Waiting for packets ..\n");

    nrf_802154_configure(false);
    recieve_();

    while (true) {
        if (cmd_pending) {
            cmd_pending = false;
            test_cmd(pending_cmd);
        }
        if (resignal_receive) {
            resignal_receive = false;
            recieve_();
        }
    }
}
