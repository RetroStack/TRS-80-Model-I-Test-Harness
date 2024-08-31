#ifndef memory_tests_h
#define memory_tests_h

#include "utils.h"

// Function prototypes
void reportFaultSRAM(uint16_t address, uint8_t expected, uint8_t actual, bool hasLowerCase);
void testWalkingOnes(uint16_t startAddress, uint16_t length, bool hasLowerCase = false);
void vramTestCheckerboard(uint16_t startAddress, uint16_t endAddress, bool hasLowerCasee);
void vramTestZeroOne(uint16_t startAddress, uint16_t endAddress, bool hasLowerCase);

#endif