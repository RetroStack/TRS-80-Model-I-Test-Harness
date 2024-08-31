#include "memory_tests.h"
#include "video.h"

// Function to report fault
void reportFaultSRAM(uint16_t address, uint8_t expected, uint8_t actual, bool hasLowerCase) {
  sprintf(stringBuffer, "Fault detected at address 0x%04X - ", address);
  Serial.print(stringBuffer);
  sprintf(stringBuffer, "Expected: 0x%02X, Actual: 0x%02X", expected, actual);
  printLine(stringBuffer);

  // Determine which chip has the fault
  for (int bit = 0; bit < 8; bit++) {
    uint8_t bitMask = 1 << bit;
    if ((expected & bitMask) != (actual & bitMask)) {
      if (bitMask == 6) {
        if ((expected & bitMask) & !hasLowerCase) {
          printLine("Error: Bit 6 active with no lowercase mod detected.\n");
          continue; // Skip reporting chip fault for bit 6
        }
      }

      // SRAM chip mapping
      // const char* sramChips[8] = {"Z63", "Z62", "Z61", "Z45", "Z46", "Z47", "Piggy back", "Z48"};
      sprintf(stringBuffer, "Chip '%s' has a fault\n", sramChips[bit]);
      printLine(stringBuffer);

      if (0) {
        // DRAM chip mapping
        // const char* dramChips[8] = {"Z13", "Z14", "Z220", "Z15", "Z19", "Z18", "Z16", "Z17"};
        sprintf(stringBuffer, "Chip %s has a fault\n", dramChips[bit]);        
        printLine(stringBuffer);
      }
    }
  }
}

// Zero-One Test function
void vramTestZeroOne(uint16_t startAddress, uint16_t endAddress, bool hasLowerCase) {
  uint16_t address = 0;
  int faultCount = 0;

  // TODO: FF will fail video memory if LC chip not present
  uint8_t testPatterns[2] = {0x00, 0xFF}; // Patterns to test (all 0s and all 1s)
  // uint8_t testPatterns[2] = {0x00, 0xBF}; // Patterns to test (all 0s and all 1s)
  bool errorsFound = false;

  printLine(F("Zero-One memory test for SRAM/VRAM"));

  // noInterrupts();

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  // Shifting 1 hex values: 1, 2, 4, 8, 10, 20, 40, 80
  // Write patterns to memory
  for (int pattern = 0; pattern < 2; pattern++) {
    uint8_t expected = testPatterns[pattern];

    Serial.print("ZeroOnes pattern value: ");
    Serial.println(pattern, HEX);    
    setAddressLinesToOutput(0x3c00);
    setDataLinesToOutput();

    // Write pattern to each byte in memory within the specified range
    for (address = startAddress; address <= endAddress; address++) {
      writeByteVRAM(address, expected);
    }

    pinMode(WR_L, OUTPUT);
    digitalWrite(WR_L, HIGH);
    pinMode(WR_L, INPUT);
    setAddressLinesToOutput(0x3c00);
    setDataLinesToInput();

    // Verify pattern
    for (address = startAddress; address <= endAddress; address++) {
      uint8_t actual = readByteVRAM(address);

      if (address >= 0x3D00 && false) {
        Serial.print(address, HEX);
        Serial.print(" ");
        Serial.print(expected, HEX);
        Serial.print(" ");
        Serial.println(actual, HEX);
      }

      if (actual != expected) {
        // TODO: lowercase check needs to go in
        if ((actual &= 0xBF) == (expected &= 0xBF)) {
          continue;
        }
        // Serial.print(address, HEX);
        // printLine(" ", actual, " vs ", expected);
        faultCount++;
        // Report fault
        reportFaultSRAM(address, expected, actual, hasLowerCase);
        errorsFound = true;
      }
    }

    asmWait(55000);
  }

  // prep for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    pinMode(RD_L, INPUT);
    pinMode(WR_L, INPUT);
    pinMode(RAS_L, INPUT);

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }

  cls();
  displayString(0x3c00, "TEST COMPLETE, CHECK CONSOLE FOR STATUS.");
  interrupts();

  printLine(F("Test complete, found "), faultCount, F(" faults."));
  if (!errorsFound) {
    printLine("No errors were found in the SRAM/VRAM memory test.\n");
  }

}

// Checkerboard Pattern Test function
void vramTestCheckerboard(uint16_t startAddress, uint16_t endAddress, bool hasLowerCase) {
  uint16_t address;
  // uint8_t testPatterns[2] = {0xAA, 0x55}; // Checkerboard patterns
  uint8_t testPatterns[2] = {0xAA, 0x15}; // Checkerboard patterns  
  bool errorsFound = false;

  printLine(F("Checkerboard pattern memory test for SRAM/VRAM."));
  delay(500);

  noInterrupts();

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }
  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();

  // Write checkerboard patterns to memory with bit 6 fixed
  for (int pattern = 0; pattern < 2; pattern++) {
    uint8_t expected = testPatterns[pattern] & ~0x40;  // Clear bit 6 (sixth bit, 0x40)

    // Write pattern to each byte in memory within the specified range
    for (address = startAddress; address <= endAddress; address++) {
      writeByteVRAM(address, expected);
      expected = (~expected & ~0x40) | (testPatterns[pattern] & 0x40); // Toggle all but bit 6
    }

    readVRAM(false, false);
    asmWait(6000, 600);

    // Verify pattern
    expected = testPatterns[pattern] & ~0x40;  // Clear bit 6
    for (address = startAddress; address <= endAddress; address++) {
      uint8_t actual = readByteVRAM(address);
      if (actual != expected) {
        // Report fault
        reportFaultSRAM(address, expected, actual, hasLowerCase);
        errorsFound = true;
      }
      expected = (~expected & ~0x40) | (testPatterns[pattern] & 0x40); // Toggle all but bit 6
    }

    asmWait(55000);
  }


  // prep for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    pinMode(RD_L, INPUT);
    pinMode(WR_L, INPUT);
    pinMode(RAS_L, INPUT);

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }

  cls();
  displayString(0x3c00, "TEST COMPLETE.");

  interrupts();

  if (!errorsFound) {
    printLine(F("No errors were found in SRAM/VRAM memory test."));
  }
}

void testWalkingOnes_x(uint16_t startAddress, uint16_t length, bool hasLowerCase) {
  Serial.println("Running Walking 1s Test...");

  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();


  for (uint8_t bit = 0; bit < 8; bit++) {
    if (bit == 6 && !hasLowerCase) continue; // Skip bit 6 if hasLowerCase is false
    uint8_t bitPattern = (1 << bit);
    for (uint16_t addr = startAddress; addr < startAddress + length; addr++) {
      // Write 0x00 to the address
      // writeMemory(addr, 0x00);
      writeByteVRAM(addr,0x00);
      // Verify it's 0x00
      // if (readMemory(addr) != 0x00) {
      if (readByteVRAM(addr) != 0x00) {
        Serial.print("Error at address ");
        Serial.print(addr, HEX);
        Serial.print(": should be 0x00, failed bit ");
        Serial.println(bit, DEC);
        return;
      }

      asmWait(65535,15);

      // Write the bit pattern
      writeByteVRAM(addr, bitPattern);
      // Verify it's the bit pattern
      if (readByteVRAM(addr) != bitPattern) {
        Serial.print("Error at address ");
        Serial.print(addr, HEX);
        Serial.print(": should be ");
        Serial.print(bitPattern, HEX);
        Serial.print(", failed bit ");
        Serial.println(bit, DEC);
        return;
      }

      asmWait(65535, 15);

      // Reset to 0x00
      writeByteVRAM(addr, 0x00);
    }
  }
  Serial.println("Walking 1s test passed.");
}

// Walking bit 1, a visual test
void testWalkingOnes(uint16_t startAddress, uint16_t length, bool hasLowerCase) {
  Serial.println("--- Walking 1's Test ---");
  cls();
  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();

  for (uint16_t addr = startAddress; addr < startAddress + length; addr++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (bit == 6 && !hasLowerCase) continue; // Skip bit 6 if hasLowerCase is false
      uint8_t bitPattern = (1 << bit);
      // Write 0x00 to the address
      writeByteVRAM(addr, 0x00);

      // Verify it's 0x00
      if (readByteVRAM(addr) != 0x00) {
        Serial.print(F("Error at address 0x"));
        Serial.print(addr, HEX);
        Serial.print(F(": should be 0x00, failed bit "));
        Serial.println(bit, DEC);
      }
      asmWait(65535, 4);

      // Write the bit pattern
      writeByteVRAM(addr, bitPattern);
      Serial.print(addr, HEX);
      Serial.print(": ");
      Serial.println(bitPattern, BIN);
      asmWait(65535, 4);
    }
  }
}


