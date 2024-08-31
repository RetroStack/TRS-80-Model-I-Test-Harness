#ifndef vram_h
#define vram_h

#include <stdint.h>
#include "shield_pins.h"
#include "utils.h"

#define VIDEO_MEM_START   0x3C00
#define VIDEO_MEM_SIZE    1024
#define VIDEO_COLS        64
#define VIDEO_ROWS        16


// void fillVRAM(uint8_t fillValue, bool silent = true);
void fillVRAM(uint8_t fillValue = 0x20, bool silent = true, uint16_t = 0x3C00, uint16_t end = 0x3FFF);
void fillVRAMwithPattern(uint8_t* pattern, bool silent, uint16_t start = 0x3C00, uint16_t end = 0x3FFF);
// void fillVRAMwithPattern(const char* pattern, bool silent, uint16_t start = 0x3C00, uint16_t end = 0x3FFF);
// void fillVRAMwithPattern(const uint8_t* pattern, int patternSize, bool silent, uint16_t start, uint16_t end);

uint32_t readVRAM(bool silent = false, bool showInHex = true);
bool checkVRAMFillWithChecksum(uint8_t fillValues[], int fillValuesCount);
void cls();
bool compareVRAMChecksum(uint32_t checksum);
void displayCharacterSet();
void displayString(uint16_t startAddress, const char *message);
void handleVideoMenu();
void handleW();
void handleFxx(int hexValue);
void handleFxxStartEnd(int hexValue, int startAddress, int endAddress);
bool lowercaseModExists();
void printVideoUtilitiesMenu();
void vramTestZeroOne();
void vramTest1();
void vramTest2();
uint8_t readByteVRAM(uint16_t memAddress);
void writeByteVRAM(uint16_t memAddress, uint8_t data);
void zerosOnes();

#endif

