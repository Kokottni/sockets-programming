#pragma once

#include <stddef.h>
#include <stdint.h>

/*  Packet structure
    ----------------

         0        1        2       3       4       5
        +--------+--------+-------+-------+-------+-------+
        | Packet | Seq No | Data  | Data  | CRC   | CRC   |
        |  Type  |        |       |       |       |       |
        +--------+--------+-------+-------+-------+-------+

*/

#define PKT_SIZE 6
#define PKT_DATA_SIZE 2

#define PKT_TYPE_OFFSET 0
#define PKT_SEQNUM_OFFSET 1
#define PKT_DATA_OFFSET 2
#define PKT_CRC_OFFSET PKT_DATA_OFFSET + PKT_DATA_SIZE

#define PKT_TYPE_DATA 1
#define PKT_TYPE_ACK 2
#define PKT_TYPE_NAK 3

/* Not as bad as it looks...
 * We have a struct which has members for each part of the packet, as described
 * by the comments. However, we also may wish to access or use that memory/data
 * in the form of a raw byte array. A union allows us to do this: access the same
 * region of memory as multiple different types. In our case, a struct, and a byte
 * array. This usage is called type punning.

 * The packed attribute ensures we don't get awkward padding in our struct.
 *
 * Ex:
 *   packet_t mypkt;
 *   build_packet(...)
 *   mypkt.type = PKT_TYPE_DATA;        // we access normal members as usual for a struct
 *   mypkt.bytes[0] == PKT_TYPE_DATA;   // we can access that same data as raw bytes, too */
typedef struct {
    union {
        struct {
            /* the type of packet, PKT_TYPE_{DATA, ACK, NAK} as defined in macros above */
            uint8_t type;

            /* the sequence number of this packet */
            uint8_t sequence_number;

            /* payload */
            uint8_t data[PKT_DATA_SIZE];

            /* crc checksum*/
            uint16_t crc_sum;
        };

        uint8_t bytes[PKT_SIZE];
    };
} __attribute__((packed)) packet_t;

/* Builds a packet by editing the (probably empty) packet_t.
 *   The type field is expected to be one of the defined packet types.
 *   The data array is expected to be the size of the defined DATA_LENGTH.
 *   num is the packet number of the current packet.
 */
void pkt_build(packet_t *packet, unsigned char type, char data[], unsigned char num);

/* Prints the given packet in a neat one-line format */
void pkt_print(packet_t *packet);

/* Returns the packet in host byte order */
packet_t pkt_ntoh(packet_t pkt);

/* Returns the packet in network byte order */
packet_t pkt_hton(packet_t pkt);

/* Given a byte-array, data, of length len, iterates over each byte and
 * introduces bit-error based on the bit error rate (BER), p

 * Warning:
 *   Modifies data in place! Make sure to have a copy of your
 *   if you need it later  */
void introduce_bit_error(uint8_t *data, size_t len, double p);