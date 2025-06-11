#include "crc.h"

/* note: we could speed this up with a table lookup version
 * for more details on CRC calculation see:
 * https://barrgroup.com/blog/crc-series-part-3-crc-implementation-code-cc */
uint16_t crc_calculate(const uint8_t *data, size_t length) {
    uint16_t crc = INIT_CRC;

    // work by byte
    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;

        // work by bit
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ GEN_POLY;
            } else {
                crc = crc << 1;
            }
        }
    }

    // return remainder
    return crc;
}