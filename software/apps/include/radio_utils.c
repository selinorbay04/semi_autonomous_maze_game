#include "radio_utils.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>


void nrf_802154_configure(bool sender) {
    if (sender) {
        printf("About to init for sender\n");
        nrf_802154_init();
        printf("Done with init for sender\n");
        nrf_802154_channel_set(11);
        uint8_t src_pan_id[] = {0xcd, 0xab};
        nrf_802154_pan_id_set(src_pan_id);
        printf("Radio configured!\n");
    } else {
        printf("About to init for receiver\n");
        nrf_802154_init();
        printf("Done with init for receiver\n");
        nrf_802154_channel_set(11);
        nrf_802154_auto_ack_set(false);
        nrf_802154_promiscuous_set(true);
        uint8_t src_pan_id[] = {0xcd, 0xab};
        nrf_802154_pan_id_set(src_pan_id);
        printf("Radio configured!\n");
        uint8_t extended_addr[] = {0x50, 0xbe, 0xca, 0xc3, 0x3c, 0x36, 0xce, 0xf4};
        nrf_802154_extended_address_set(extended_addr);
    }
}


uint8_t recieve_pkt_raw(uint8_t* pkt) {
    uint8_t payload = pkt[24]; 
    printf("Packet: [ ");
    for (int i = 0; i < pkt[0] - 1; i++) {
        printf("%02X ", pkt[i]);
    }
    printf("]\n");
    nrf_802154_buffer_free_raw(pkt);
    return payload;
}

void recieve_(void) {
    if (nrf_802154_receive()) {
        printf("Entered receive mode\n");
    } else {
        printf("Could not enter receive mode\n");
    }
}

uint8_t* load_pkt(uint8_t payload, uint8_t size) {
    static uint8_t pkt[PSDU_MAX_SIZE];

    uint8_t src_pan_id[]        = {0xcd, 0xab};
    uint8_t src_extended_addr[] = {0xdc, 0xa9, 0x35, 0x7b, 0x73, 0x36, 0xce, 0xf4};
    uint8_t dst_extended_addr[] = {0x50, 0xbe, 0xca, 0xc3, 0x3c, 0x36, 0xce, 0xf4};

    nrf_802154_extended_address_set(src_extended_addr);

    pkt[0] = 26 + FCS_LENGTH; 
    pkt[1] = 0x01;            /* Frame Control Field low  */
    pkt[2] = 0xcc;            /* Frame Control Field high */
    pkt[3] = 0x00;            /* Sequence number          */
    pkt[4] = 0xff;            /* Destination PAN ID low   */
    pkt[5] = 0xff;            /* Destination PAN ID high  */
    memcpy(&pkt[6],  dst_extended_addr, 8); /* Destination extended address */
    memcpy(&pkt[14], src_pan_id, 2);        /* Source PAN ID                */
    memcpy(&pkt[16], src_extended_addr, 8); /* Source extended address      */
    pkt[24] = payload;                       /* Single-byte payload          */

    return pkt;
}

void send_pkt(uint8_t* pkt) {
    if (nrf_802154_transmit_raw(pkt, true)) {
        printf("Packet sent!\n");
    } else {
        printf("Failed to send packet\n");
    }
}

void send_radio_command(uint8_t command_byte) {
    uint8_t* pkt = load_pkt(command_byte, 1);
    send_pkt(pkt);
}

// callbacks required by the driver

void nrf_802154_transmitted_raw(const uint8_t* p_frame, uint8_t* p_ack, int8_t power, uint8_t lqi) {
    printf("TX success\n");
}

void nrf_802154_transmit_failed(const uint8_t* p_frame, nrf_802154_tx_error_t error) {
    printf("TX failed: %d\n", (int)error);
}

void nrf_802154_tx_started(const uint8_t* p_frame) {
    printf("TX started\n");
}
