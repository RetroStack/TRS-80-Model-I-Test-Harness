#ifndef ROMS_H
#define ROMS_H

#include <stdint.h>
#include <Arduino.h>
#include <CRC32.h>
#include "shield_pins.h"

// Define a structure to represent each ROM entry
typedef struct {
    char version[5]; // ROM version
    unsigned int checksumA; // Checksum for ROM A
    unsigned int checksumB; // Checksum for ROM B
    unsigned int checksumC; // Checksum for ROM C
    unsigned long crc32; // CRC-32 value
    char valid[2]; // Validity ("Yes" or "No")
} M1_ROMs;

void getROMInfo(bool);

#endif