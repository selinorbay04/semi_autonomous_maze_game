

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_802154.h"
#include "radio_utils.h"

int main(void) {
    nrf_delay_ms(100);
    printf("Radio send test running!\n");
    printf("Will alternate TURN_LEFT (0x%02X) / TURN_RIGHT (0x%02X) every 2s\n",
           TURN_LEFT, TURN_RIGHT);

    nrf_802154_configure(true);

    uint32_t count = 0;

    while (true) {
        uint8_t cmd  = (count % 2 == 0) ? TURN_LEFT : TURN_RIGHT;
        const char* label = (cmd == TURN_LEFT) ? "TURN_LEFT" : "TURN_RIGHT";

        printf("[%lu] Sending %s (0x%02X)...\n", count, label, cmd);

        uint8_t* pkt = load_pkt(cmd, 1);
        send_pkt(pkt);

        count++;
        nrf_delay_ms(2000);
    }
}
