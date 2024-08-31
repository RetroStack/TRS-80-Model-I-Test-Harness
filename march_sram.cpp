#include "march_sram.h"

#define MEMORY_SIZE 1024

// Function prototypes
void testSRAM(uint16_t memoryAddress, uint16_t memorySize, uint32_t *faultsFoundCount, bool hasLowercase, uint16_t *totalBytesTested, uint32_t *totalBitsTested, bool silent = true);
bool checkOtherBitsZero(uint16_t memoryAddress, uint16_t memorySize, uint16_t currentMemoryAddress, uint8_t currentBit, bool hasLowercase, uint32_t *faultsFoundCount, bool silent = true);
void testSRAMInverse(uint16_t memoryAddress, uint16_t memorySize, uint32_t *faultsFoundCount, bool hasLowercase, uint16_t *totalBytesTested, uint32_t *totalBitsTested, bool silent = true);
bool checkOtherBitsOne(uint16_t memoryAddress, uint16_t memorySize, uint16_t currentMemoryAddress, uint8_t currentBit, bool hasLowercase, uint32_t *faultsFoundCount, bool silent = true);


int marchTestSRAM() {
    bool hasLowercase = true;
    uint32_t faultsFoundCount = 0;
    uint16_t videoAddress = 0x3C00;
    uint16_t memSize = 1024;
    uint16_t totalBytesTested = 0;
    uint32_t totalBitsTested = 0;
    // uint32_t totalBadBits;     // TODO: this requires an set to store to prevent double counting

    memoryChipSet[0].clear();

    // Test the memory (Zeros test)
    printLine("March Test");
    sprintf(stringBuffer, "March Test\nStarting: 0x%04X, size: %i", videoAddress, memSize);
    printLine(stringBuffer);
    printLine("Write 0, Check for 0, Verify for 0, Write 1, Verify for 0");
    testSRAM(videoAddress, memSize, &faultsFoundCount, hasLowercase, &totalBytesTested, &totalBitsTested);
 
    if (0) {
      // Test the memory (Ones test)
      printLine("Write 1, Check for 1, Verify for 1, Write 0, Verify for 1");
      videoAddress = 0x3c00;
      // memSize = 3;
      testSRAMInverse(videoAddress, memSize, &faultsFoundCount, hasLowercase, &totalBytesTested, &totalBitsTested, false);
    } 

    // Print summary
    sprintf(stringBuffer, "Total bytes tested: %u", (unsigned int)totalBytesTested);
    printLine(stringBuffer);
    sprintf(stringBuffer, "Total bits tested: %lu", (unsigned long)totalBitsTested);
    printLine(stringBuffer);

    // Check if there are zero faults found
    if (faultsFoundCount == 0) {
      printLine("SRAM test passed with no faults found.");
    } else {
      // TODO: list out which chips failed
      sprintf(stringBuffer, "SRAM test failed - faults: %lu\n", (unsigned long)faultsFoundCount);
      printLine(stringBuffer);
      Serial.print(F("Check IC chips: "));
      memoryChipSet[0].print();
      Serial.println(" ");
    }

    return 0;
}

// Test's the SRAM. Hint: 20H (space) is not 00H, 00H on the M1 shows up as '@'  
// Fill with 0's and walk a 1 (bit) from low to high memory space
// TODO: put in feature if 's' is pressed skips to next line
void testSRAM(uint16_t memoryAddress, uint16_t memorySize, uint32_t *faultsFoundCount, bool hasLowercase, uint16_t *totalBytesTested, uint32_t *totalBitsTested, bool silent) {
    *faultsFoundCount = 0;
    *totalBytesTested = 0;
    *totalBitsTested = 0;

    // hasLowercase = false;
    hasLowercase = true;

    if (!hasLowercase) {
      SILENT_PRINTLN(F("Skipping bit 6 test as no lowercase detected."));
    }

    // Step 1: Initialize the memory to 0
    sprintf(stringBuffer, "Zeroing buffer from 0x%04X to 0x%04X", memoryAddress, memoryAddress + memorySize - 1);
    SILENT_PRINTLN(silent, stringBuffer);
    fillVRAM(0x0, true, memoryAddress, memoryAddress + memorySize - 1);

    // Step 2: Test each bit
    for (uint16_t videoAddress = memoryAddress; videoAddress < (memoryAddress + memorySize); videoAddress++) {
      (*totalBytesTested)++;
      sprintf(stringBuffer, "Testing Address: 0x%04X, %i bad SRAMs: ", videoAddress, memoryChipSet[0].getSize());
      Serial.print(stringBuffer);
      memoryChipSet[0].print();
      Serial.println(" ");

      for (uint8_t bitIndex = 0; bitIndex < 8; bitIndex++) {
        if (bitIndex == 6 && !hasLowercase) {   // ignore bit if no lowercase mod as extra SRAM chip required
            continue;
        }
        (*totalBitsTested)++;   // update count
        sprintf(stringBuffer, "\tBit: %d, chip: %s", bitIndex, sramChips[bitIndex]);        
        SILENT_PRINTLN(silent, stringBuffer);
  
        // Read the bit (should be 0)
        setAddressLinesToOutput(videoAddress);
        setDataLinesToInput();

        // uint8_t bitValue = (memory[byteIndex] >> bitIndex) & 0x01;
        uint8_t bitValue = (readByteVRAM(videoAddress) >> bitIndex) & 0x01;
        SILENT_PRINT(silent, F("\tExpecting 0, read "));
        SILENT_PRINT(silent, bitValue);

        if (bitValue != 0) {
          SILENT_PRINTLN(silent, F(" - FAIL"));
          // printf("Failure detected at memory address: 0x%04X, bit: %d (%s)\n", byteIndex, bitIndex, sramChips[bitIndex]);
          // faultsFound[*faultsFoundCount] = (byteIndex << 3) | bitIndex;
          (*faultsFoundCount)++;
          memoryChipSet[0].add(sramChips[bitIndex]);
        } else {
          SILENT_PRINTLN(silent, F(" - OK"));
        }

        // Write 1 to the bit
        setAddressLinesToOutput(videoAddress);
        setDataLinesToOutput();
        // memory[byteIndex] |= (1 << bitIndex);
        sprintf(stringBuffer, "\tWriting 1 to bit %i - checking memory space for corruption.", bitIndex);
        SILENT_PRINTLN(silent, stringBuffer);
        writeByteVRAM(videoAddress, 1 << bitIndex);

        // TODO: read back to see if 1 is there? 

        // Check all other bits in the entire memory space are still 0
        // memorySize = 3;
        checkOtherBitsZero(memoryAddress, memorySize, videoAddress, bitIndex, hasLowercase, faultsFoundCount, true);

        // Restore the bit to 0
        setAddressLinesToOutput(videoAddress);
        setDataLinesToOutput();
        // memory[byteIndex] &= ~(1 << bitIndex);
        writeByteVRAM(videoAddress, 0);

        // TODO: do we need to check if the bit really is zero?
      }
    }
}

/**
 * Iterates through bytes in the memory range and checks if all bits
 * are zero except the bit specified by currentByte and currentBit. If any bit 
 * other than the specified bit is found to be non-zero, logs the failure.
 */
bool checkOtherBitsZero(uint16_t memoryAddress, uint16_t memorySize, uint16_t currentMemoryAddress, uint8_t currentBit, bool hasLowercase, uint32_t *faultsFoundCount, bool silent) {
  bool result = true;

  setAddressLinesToOutput(memoryAddress);
  setDataLinesToInput();    

  sprintf(stringBuffer, "\tcurrentMemoryAddress: 0x%04X, currentBit: %d", currentMemoryAddress, currentBit);
  SILENT_PRINTLN(silent, stringBuffer);

  for (uint16_t videoAddress = memoryAddress; videoAddress < memoryAddress + memorySize; videoAddress++) {
    uint8_t byteValue = readByteVRAM(videoAddress);

    for (uint8_t bitIndex = 0; bitIndex < 8; bitIndex++) {
      sprintf(stringBuffer, "\tVerifying memory address: 0x%04X, bit: %d", videoAddress, bitIndex);
      SILENT_PRINTLN(silent, stringBuffer);

      // check if lowecase bit needs to be checked
      if (!hasLowercase && bitIndex == 6) {
        sprintf(stringBuffer, "\tNo LC kit, ignoring memory address: 0x%04X, bit: %d", videoAddress, bitIndex);
        SILENT_PRINTLN(silent, stringBuffer);

        continue;
      }

      if (videoAddress == currentMemoryAddress && bitIndex == currentBit) {
        sprintf(stringBuffer, "\tIgnoring memory address and bit position: 0x%04X, bit: %d", videoAddress, bitIndex);
        SILENT_PRINTLN(stringBuffer);

      } else {
        if ((byteValue >> bitIndex) & 0x01) {
          sprintf(stringBuffer, "\t** Failure detected at memory address: 0x%04X, bit: %d (%s)", videoAddress, bitIndex, sramChips[bitIndex]);
          SILENT_PRINTLN(stringBuffer);
          sprintf(stringBuffer, "\t** Read back: %02X ", byteValue);
          SILENT_PRINTLN(stringBuffer);
          (*faultsFoundCount)++;
          memoryChipSet[0].add(sramChips[bitIndex]);
          result = false;
        }
      }
    }
  }
  return result;
}

void testSRAMInverse(uint16_t memoryAddress, uint16_t memorySize, uint32_t *faultsFoundCount, bool hasLowercase, uint16_t *totalBytesTested, uint32_t *totalBitsTested, bool silent) {
  *faultsFoundCount = 0;
  *totalBytesTested = 0;
  *totalBitsTested = 0;

  hasLowercase = false;

  if (!hasLowercase) {
    SILENT_PRINTLN(F("Skipping bit 6 test as no lowercase detected."));
  }

  // Step 1: Initialize the memory to 1
  sprintf(stringBuffer, "Setting buffer to 1 from 0x%04X to 0x%04X", memoryAddress, memoryAddress + memorySize - 1);
  SILENT_PRINTLN(silent, stringBuffer);
  fillVRAM(0xFF, true, memoryAddress, memoryAddress + memorySize - 1);

  // Step 2: Test each bit
  for (uint16_t videoAddress = memoryAddress + memorySize - 1; videoAddress >= memoryAddress; videoAddress--) {
    (*totalBytesTested)++;
    sprintf(stringBuffer, "Testing Address: 0x%04X", videoAddress);              
    printLine(stringBuffer);

    for (int8_t bitIndex = 7; bitIndex >= 0; bitIndex--) {
      if (bitIndex == 6 && !hasLowercase) {   // ignore bit if no lowercase mod as extra SRAM chip required
        continue;
      }
      (*totalBitsTested)++;   // update count
      sprintf(stringBuffer, "\tBit: %d, chip: %s", bitIndex, sramChips[bitIndex]);
      SILENT_PRINTLN(silent, stringBuffer);

      // Read the bit (should be 1)
      setAddressLinesToOutput(videoAddress);
      setDataLinesToInput();

      uint8_t currentValue = readByteVRAM(videoAddress);
      uint8_t bitValue = (currentValue >> bitIndex) & 0x01;
      SILENT_PRINT(silent, F("\tExpecting 1, read "));
      SILENT_PRINT(silent, bitValue);

      if (bitValue != 1) {
        SILENT_PRINTLN(silent, F(" - FAIL"));
        (*faultsFoundCount)++;
        memoryChipSet[0].add(sramChips[bitIndex]);
      } else {
        SILENT_PRINTLN(silent, F(" - OK"));
      }

      // Write 0 to the bit
      setAddressLinesToOutput(videoAddress);
      setDataLinesToOutput();
      uint8_t newValue = currentValue & ~(1 << bitIndex);
      sprintf(stringBuffer, "\tWriting 0 to bit %i - checking memory space for corruption.", bitIndex);
      SILENT_PRINTLN(silent, stringBuffer);
      writeByteVRAM(videoAddress, newValue);

      // Check all other bits in the entire memory space are still 1
      checkOtherBitsOne(memoryAddress, memorySize, videoAddress, bitIndex, hasLowercase, faultsFoundCount, silent);

      // Restore the bit to 1
      setAddressLinesToOutput(videoAddress);
      setDataLinesToOutput();
      newValue = currentValue | (1 << bitIndex);
      writeByteVRAM(videoAddress, newValue);
    }
  }
}

/**
 * Iterates through bytes in the memory range and checks if all bits
 * are one except the bit specified by currentByte and currentBit. If any bit 
 * other than the specified bit is found to be non-one, logs the failure.
 */
bool checkOtherBitsOne(uint16_t memoryAddress, uint16_t memorySize, uint16_t currentMemoryAddress, uint8_t currentBit, bool hasLowercase, uint32_t *faultsFoundCount, bool silent) {
  bool result = true;

  setAddressLinesToOutput(memoryAddress);
  setDataLinesToInput();

  sprintf(stringBuffer, "\tcurrentMemoryAddress: 0x%04X, currentBit: %d", currentMemoryAddress, currentBit);
  SILENT_PRINTLN(silent, stringBuffer);

  for (uint16_t videoAddress = memoryAddress + memorySize - 1; videoAddress >= memoryAddress; videoAddress--) {
    uint8_t byteValue = readByteVRAM(videoAddress);

    for (int8_t bitIndex = 7; bitIndex >= 0; bitIndex--) {
      sprintf(stringBuffer, "\tVerifying memory address: 0x%04X, bit: %d", videoAddress, bitIndex);
      SILENT_PRINTLN(silent, stringBuffer);

      if (!hasLowercase && bitIndex == 6) {
        sprintf(stringBuffer, "\tNo LC kit, ignoring memory address: 0x%04X, bit: %d", videoAddress, bitIndex);
        SILENT_PRINTLN(silent, stringBuffer);
        continue;
      }

      if (videoAddress == currentMemoryAddress && bitIndex == currentBit) {
        sprintf(stringBuffer, "\tIgnoring memory address and bit position: 0x%04X, bit: %d", videoAddress, bitIndex);
        SILENT_PRINTLN(silent, stringBuffer);
      } else {
        if (!((byteValue >> bitIndex) & 0x01)) {
          sprintf(stringBuffer, "\t** Failure detected at memory address: 0x%04X, bit: %d", videoAddress, bitIndex);
          SILENT_PRINTLN(silent, stringBuffer);
          sprintf(stringBuffer, "\t** Read back: %02X ", byteValue);
          SILENT_PRINTLN(silent, stringBuffer);
          (*faultsFoundCount)++;
          memoryChipSet[0].add(sramChips[bitIndex]);
          result = false;
        }
      }
    }
  }
  return result;
}

