#include "pkt_utils.h"
#include "crc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

void pkt_build(packet_t *packet, unsigned char type, char data[], unsigned char num) {
    // clear packet
    memset(packet, 0, PKT_SIZE);

    packet->type = type;
    packet->sequence_number = num;

    memcpy(packet->data, data, PKT_DATA_SIZE);

    // calc and store crc in network byte order
    uint16_t crc = crc_calculate(packet->bytes, PKT_SIZE - 2);
    packet->crc_sum = htons(crc);
}

packet_t pkt_ntoh(packet_t pkt) {
    pkt.crc_sum = ntohs(pkt.crc_sum);
    return pkt;
}

packet_t pkt_hton(packet_t pkt) {
    pkt.crc_sum = htons(pkt.crc_sum);
    return pkt;
}

void pkt_print(packet_t *packet) {
    // print packet type
    switch (packet->type) {
    case PKT_TYPE_DATA:
        printf("[DAT");
        break;
    case PKT_TYPE_ACK:
        printf("[ACK");
        break;
    case PKT_TYPE_NAK:
        printf("[NAK");
        break;
    default:
        printf("[???");
    }

    // print sequence number
    printf("|%d|", packet->sequence_number);

    // print packet data
    for (int i = 0; i < PKT_DATA_SIZE; i++) {
        if (packet->data[i] == '\0')
            printf(" ");
        else
            printf("%c", packet->data[i]);
    }

    // print CRC
    printf("|%x]\n", packet->crc_sum);
}

static void introduce_bit_error_in_byte(uint8_t *data, double p) {
    char c = 0x01;
    for (int i = 0; i < 8; i++) {
        if ((double)rand() / RAND_MAX <= p) {
            *data ^= c;
        }
        c = c << 1;
    }
}

void introduce_bit_error(uint8_t *data, size_t len, double p) {
    for (size_t i = 0; i < len; i++) {
        introduce_bit_error_in_byte(&data[i], p);
    }
}
