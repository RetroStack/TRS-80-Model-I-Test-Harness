#include "memory.h"

void printMemoryMenu() {
    Serial.println(F("*** Memory Utilities ***  TO BE IMPLEMENTED ***"));
    Serial.println(F("\t1 - ZeroOne memory test"));
    Serial.println(F("\t2 - Checkerboard memory test"));
    Serial.println(F("\t3 - WalkingOnes memory test"));
    Serial.println(F("\t4 - March test"));
    Serial.println(F("\tb - Byte read|write: r,xxxx or w,xxxx,cc"));
    Serial.println(F("\tc - Clear screen"));
    Serial.println(F("\td - Display memory to screen"));
    Serial.println(F("\tf - Font display"));    
    Serial.println(F("\tr - Read DRAM and send to serial port (HEX)"));
    Serial.println(F("\tR - Read DRAM and send to serial port (ASCII)"));
    Serial.println(F("\tu - Upload data from serial port"));
    Serial.println(F("\tw - Write pattern to DRAM"));
    Serial.println(F("\tx - Return to main menu"));
    Serial.println(F("\t? - Prints this menu"));
}

void memoryTest2() {
  Serial.println(F("Performing Memory Test 2"));
}

/*
  Read byte from memory (DRAM)
  - Arduino address lines should be set for output
  - Arduino data lines should be set for input
*/
uint8_t readByteMemory(uint16_t memAddress) {
  uint8_t data = 0;

  uint8_t lowerByte = (uint8_t)(memAddress & 0xFF); // Masking to get the lower byte
  uint8_t upperByte = (uint8_t)(memAddress >> 8);   // Shifting to get the upper byte

  PORTA = lowerByte;    // Write the lower byte of the address to PORTA
  PORTC = upperByte;    // Write the upper byte of the address to PORTC

  // RAS*
  pinMode(RAS_L, OUTPUT);
  digitalWrite(RAS_L, LOW);
  asmWait(1);

  // RD*
  pinMode(RD_L, OUTPUT);
  digitalWrite(RD_L, LOW);
  asmWait(3);

  // get data
  data = PINF;

  // prep for exit
  turnOffReadWriteRASLines();

  return data;
}
