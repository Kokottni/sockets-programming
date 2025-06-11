#pragma once

#include <stddef.h>
#include <stdint.h>

// CRC-16/AUTOSAR, CRC-16/CCITT-FALSE:
// https://reveng.sourceforge.io/crc-catalogue/16.htm#crc.cat.crc-16-ccitt-false
#define GEN_POLY 0x1021
#define INIT_CRC 0xFFFF

/* Calculates CRC remainder for given data of given length.
 * Returns the CRC remainder. */
uint16_t crc_calculate(const uint8_t *data, size_t length);