#include "Arduino.h"
#include "march_sram.h"
#include "memory_tests.h"
#include "shield_pins.h"
#include "video.h"

uint8_t videoData[VIDEO_MEM_SIZE];
uint8_t memPattern[10];

CRC32 crc;

// TODO: make this a dynamic table
uint32_t vramChecksumTable[] = {
  0xEFB5AF2E,   // 0x00
  0x24D7C38D,   // 0x20
  0xAB58F48B,   // 0x6F
  0x235141FA,   // 0x7F
  0xF58F20F3,   // 0xBF
  0xB83AFFF4    // 0xFF
};

void handleVideoMenu() {
  printVideoUtilitiesMenu();

  while (true) {
    if (Serial.available()) {
      char inChar = (char)Serial.read();
      if (inChar == '1') {
        vramTestZeroOne(0x3c00, 0x3fff, false);
      } else if (inChar == '2') {
        vramTestCheckerboard(0x3c00, 0x3fff, false);
      } else if (inChar == '3') {
        testWalkingOnes(0x3c00, 1024, false);
      } else if (inChar == '4') {
        marchTestSRAM();
      }
      else if (inChar == 'c') {
          cls();
      } else if (inChar == 'b') {
        printLine("In option b....");
        while (!Serial.available());
        readSerialInput2();
        // printLine("parameter count", parameterCount);
        // for (int i = 0; i < parameterCount; i++) {
        //   Serial.print("Parameter ");
        //   Serial.print(i);
        //   Serial.print(": ");
        //   Serial.println(parameters[i]);
        // }

        if (parameters[0][0] == 'r' && parameterCount == 2) {
          uint16_t memAddress = (uint16_t)strtol(parameters[1], NULL, 16);
          pinMode(WR_L, INPUT);
          digitalWrite(WR_L, HIGH);
          setAddressLinesToOutput(memAddress);
          setDataLinesToInput();
          uint8_t data = readByteVRAM(memAddress);
          sprintf(stringBuffer, "mem address=%04X, byte read=%02X", memAddress, data);
          Serial.println(stringBuffer);     // Print newline character
        }
         else if (parameters[0][0] == 'w' && parameterCount == 3) {
          uint16_t memAddress = (uint16_t)strtol(parameters[1], NULL, 16);
          setAddressLinesToOutput(memAddress);
          setDataLinesToOutput();
          uint8_t data = (uint8_t)strtol(parameters[2], NULL, 16);
          writeByteVRAM(memAddress, data);
          // printLine("byte write = ", data);
          setAddressLinesToInput();
          setDataLinesToInput();
        } else {
          printLine(F("Wrong format for b: b,r,xxxx or b,w,xxxx,hh"));
        }
      } else if (inChar == 'r') {
        readVRAM();
      } else if (inChar == 'R') {
        readVRAM(false, false);
      } else if (inChar == 'f') {
        displayCharacterSet();
      }
      else if (inChar == 'w') {
        handleW();
      } else if (inChar == 'x') {
          break;
      } else if (inChar == '?') {
          printVideoUtilitiesMenu();
      } else {
          Serial.println(F("Invalid input. Choose 1, 2, c, d, [f,xx], [f,xx,start,end] or x to return to main menu."));
      }
      serialFlush();
    }
  }
  Serial.println("Returning to main menu.");
  serialFlush();
}

void printVideoUtilitiesMenu() {
    Serial.println(F("*** Video RAM (VRAM/SRAM) Utilities ***"));
    Serial.println(F("\t1 - ZeroOne memory test (WIP)"));
    Serial.println(F("\t2 - Checkerboard memory test (WIP)"));
    Serial.println(F("\t3 - WalkingOnes memory test - a visual test"));
    Serial.println(F("\t4 - March test"));
    Serial.println(F("\tb - Byte read|write: r,xxxx or w,xxxx,cc"));
    Serial.println(F("\tc - Clear screen"));
    Serial.println(F("\tr - Read VRAM and send to serial port (HEX)"));
    Serial.println(F("\tR - Read VRAM and send to serial port (ASCII)"));
    Serial.println(F("\tf - Font display"));
    Serial.println(F("\tw - Write pattern to VRAM: w,pattern,start,end"));
    Serial.println(F("\tx - Return to main menu"));
    Serial.println(F("\t? - Prints this menu"));
}

void handleW() {
  printLine("In option W....");
  while (!Serial.available());
  readSerialInput2();

  printLine("parameter count", parameterCount);
  for (int i = 0; i < parameterCount; i++) {
    Serial.print("Parameter ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(parameters[i]);
  }

  if (parameterCount != 3) {
    Serial.println("Invalid input: Format: w,pattern,start,end");
    return;
  }

  uint16_t startAddress = strtoul(parameters[1], nullptr, 16);
  uint16_t endAddress = strtoul(parameters[2], nullptr, 16);
  convertHexStringToByteArray(parameters[0], memPattern);
  printLine("start=", startAddress, ", end=", endAddress);

    for (int i = 0; i < 10; i++) {
        if (memPattern[i] == 0) {
            break; // Stop printing when a zero is encountered
        }
        Serial.print("0x");
        Serial.print(memPattern[i], HEX);
        Serial.print(" ");
    }

  fillVRAMwithPattern(memPattern, false, startAddress, endAddress);
}

// new version, see if it works
void fillVRAMwithPattern(uint8_t* pattern, bool silent, uint16_t start, uint16_t end) {
  int patternSize = strlen(pattern);
  
  printLine("Pattern size = ", patternSize);
  // noInterrupts();
  SILENT_PRINT(silent, F("pattern = "));
  for (int i = 0; i < patternSize; i++) {
    SILENT_PRINT(silent, pattern[i], HEX);
  }

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  // make sure other control lines are set correct
  pinMode(RD_L, INPUT);
  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();

  // Fill VRAM
  for (uint16_t i = start; i <= end; i++) {
      // Split address
      uint8_t lowerByte = (uint8_t)(i & 0xFF); // Masking to get the lower byte
      uint8_t upperByte = (uint8_t)(i >> 8);   // Shifting to get the upper byte
      
      PORTA = lowerByte;    // Write the lower byte of the address to PORTA
      PORTC = upperByte;    // Write the upper byte of the address to PORTC

      // RAS*
      pinMode(RAS_L, OUTPUT);
      digitalWrite(RAS_L, LOW);
      asmWait(1);

      // fill VRAM with fillValue
      PORTF = pattern[i % patternSize];

      printLine(i, ", ", i % patternSize, ", pattern=", pattern[i % patternSize]);

      // WR*
      pinMode(WR_L, OUTPUT);
      digitalWrite(WR_L, LOW);
      asmWait(3);

      // Exit (keep this order)
      turnOffReadWriteRASLines();
  }
  
  // prep pins for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {

    turnOffReadWriteRASLines();

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }

  interrupts();
}

/*
  Write Cycle Waveform
    Address
    Chip Enable
    Data
    Read/Write
*/
// TODO: fix above routine and then remove this
void fillVRAM(uint8_t fillValue, bool silent, uint16_t start, uint16_t end) {
  noInterrupts();
  SILENT_PRINT(silent, F("fillValue = "));
  SILENT_PRINTLN(silent, fillValue, HEX);

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  pinMode(RD_L, INPUT);     // JIC
  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();

  // Fill VRAM
  for (uint16_t i = start; i <= end; i++) {
      // Split address
      uint8_t lowerByte = (uint8_t)(i & 0xFF); // Masking to get the lower byte
      uint8_t upperByte = (uint8_t)(i >> 8);   // Shifting to get the upper byte
      
      PORTA = lowerByte;    // Write the lower byte of the address to PORTA
      PORTC = upperByte;    // Write the upper byte of the address to PORTC

      // RAS*
      pinMode(RAS_L, OUTPUT);
      digitalWrite(RAS_L, LOW);
      asmWait(1);

      // fill VRAM with fillValue
      PORTF = fillValue;

      // WR*
      pinMode(WR_L, OUTPUT);
      digitalWrite(WR_L, LOW);
      asmWait(3);

      // Exit
      pinMode(WR_L, INPUT);
      pinMode(RAS_L, INPUT);
      asmWait(3);
  }
  
  // prep pins for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    pinMode(RD_L, INPUT);
    digitalWrite(RD_L, HIGH);
    pinMode(WR_L, INPUT);
    digitalWrite(WR_L, HIGH);
    pinMode(RAS_L, INPUT);
    digitalWrite(RAS_L, HIGH);

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }

  interrupts();
}

// clear screen
void cls()
{
  fillVRAM(' ');
}

// Show character to display
void displayCharacterSet()
{
  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  noInterrupts();
  cls();

  displayString(0x3c00, "ROM CHARACTER FONT DUMP");

  // set control lines
  pinMode(RD_L, INPUT);             // prob OK not to force it HIGH
  setAddressLinesToOutput(0x3c00);
  setDataLinesToOutput();

  // Fill VRAM
  for (int i = 0; i < 255; i++) {
    uint16_t address = i + 0x3C80;

    // Split address
    uint8_t lowerByte = (uint8_t)(address & 0xFF); // Masking to get the lower byte
    uint8_t upperByte = (uint8_t)(address >> 8);   // Shifting to get the upper byte
    
    PORTA = lowerByte;    // Write the lower byte of the address to PORTA
    PORTC = upperByte;    // Write the upper byte of the address to PORTC

    // RAS*
    pinMode(RAS_L, OUTPUT);
    digitalWrite(RAS_L, LOW);
    asmWait(1);

    // fill VRAM with fillValue
    PORTF = (uint8_t) i;

    // WR*
    pinMode(WR_L, OUTPUT);
    digitalWrite(WR_L, LOW);
    asmWait(3);

    // Exit write
    pinMode(WR_L, INPUT);
    pinMode(RAS_L, INPUT);
    asmWait(3);
  }
  
  // prep pins for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    turnOffReadWriteRASLines();

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }
  interrupts();
}

// Prints to video memory
void displayString(uint16_t startAddress, const char *message) {
  if (startAddress < VIDEO_MEM_START || startAddress > (VIDEO_MEM_START + VIDEO_MEM_SIZE)) {
    Serial.println("Invalid start address. Must be between 0x3C00 and 0x3FFF.");
    return;
  }

  // setup for write
  noInterrupts();

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  setAddressLinesToOutput(0x3C00);
  setDataLinesToOutput();

  uint16_t bufferIndex = startAddress;
  for (const char *p = message; *p != '\0'; p++) {
    if (bufferIndex >= (VIDEO_MEM_START + VIDEO_MEM_SIZE)) {
      Serial.println("Buffer overflow.");
      break;
    }

    if (*p == '\r') {  // Carriage return handling
      bufferIndex += VIDEO_COLS - (bufferIndex % VIDEO_COLS);
      if (bufferIndex >= (VIDEO_MEM_START + VIDEO_MEM_SIZE)) {
        Serial.println("Buffer overflow.");
        break;
      }
      continue;
    }

    // Split address
    uint16_t memAddress = bufferIndex;
    uint8_t lowerByte = (uint8_t)(memAddress & 0xFF); // Masking to get the lower byte
    uint8_t upperByte = (uint8_t)(memAddress >> 8);   // Shifting to get the upper byte
    
    PORTA = lowerByte;    // Write the lower byte of the address to PORTA
    PORTC = upperByte;    // Write the upper byte of the address to PORTC

    // RAS*
    pinMode(RAS_L, OUTPUT);
    digitalWrite(RAS_L, LOW);
    asmWait(1);

    // write the byte
    PORTF = *p;
    // Serial.print(memAddress, (HEX));
    // Serial.print("-");
    // Serial.println(*p, (HEX));

    // WR*
    pinMode(WR_L, OUTPUT);
    digitalWrite(WR_L, LOW);
    asmWait(3);

    // Exit write
    pinMode(WR_L, INPUT);
    pinMode(RAS_L, INPUT);
    asmWait(3);

    bufferIndex++;
  }

  // prep pins for exit
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
  interrupts();
}

/*
  Read Cycle Waveform
    Address
    Chip Enable
    Data
    Read
*/
uint32_t readVRAM(bool silent, bool showInHex) {
  uint32_t checksum = 0;

  SILENT_PRINTLN(silent, "Reading VRAM...");

  noInterrupts();

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW);
  }

  setAddressLinesToOutput(0x3c00);
  setDataLinesToInput();

  // this seems to be needed otherwise a random character gets written to the VRAM
  // at the start of the address in the loop below.
  // TODO: I wonder if wonder if write has something similar
  if (1) {
    pinMode(WR_L, OUTPUT);
    digitalWrite(WR_L, HIGH);
    pinMode(WR_L, INPUT);
  }

  for (int i = 0; i < VIDEO_MEM_SIZE; ++i) {
      // Split address
      uint16_t memAddress = 0x3c00 + i;
      uint8_t lowerByte = (uint8_t)(memAddress & 0xFF); // Masking to get the lower byte
      uint8_t upperByte = (uint8_t)(memAddress >> 8);   // Shifting to get the upper byte
      
      PORTA = lowerByte;    // Write the lower byte of the address to PORTA
      PORTC = upperByte;    // Write the upper byte of the address to PORTC

      // RAS*
      pinMode(RAS_L, OUTPUT);
      digitalWrite(RAS_L, LOW);
      asmWait(2);

      // RD*
      pinMode(RD_L, OUTPUT);  
      digitalWrite(RD_L, LOW);
      asmWait(3);

      //fill VRAM with fillValue
      videoData[i] = PINF;

      // Exit read
      // pinMode(RD_L, INPUT);
      // pinMode(RAS_L, INPUT);
      asmWait(3);
  }

  // prep pins for exit
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    turnOffReadWriteRASLines();

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }

  interrupts();

  crc.reset();
  crc.update(videoData, VIDEO_MEM_SIZE);
  checksum = crc.finalize();

  if (!silent) {
    Serial.println(F("--- Video buffer dump start ---"));

    for (int i = 0; i < VIDEO_MEM_SIZE; ++i) {
      if (i % 64 == 0) {  // Check if 64 characters have been printed
        sprintf(stringBuffer, "%04X", 0x3C00 + i);
        Serial.print(stringBuffer);     // Print newline character
        Serial.print(" ");
      }

      if (showInHex) {
        Serial.print(videoData[i], HEX);
      } else {
        Serial.print((char) videoData[i]);
      }
      if ((i + 1) % 64 == 0) {  // Check if 64 characters have been printed
        Serial.print('\n');     // Print newline character
      }
    }
    Serial.println(F("\n--- Video buffer dump end   ---"));

    Serial.print("Checksum: ");
    Serial.println(checksum, (HEX));
  }

  return checksum;
}

bool checkVRAMFillWithChecksum(uint8_t fillValues[], int fillValuesCount) {
  bool retVal = true;
  for (int i = 0; i < fillValuesCount; i++) {
    bool valid = false;
    uint32_t checksum = 0;
    // Serial.print("Filling VRAM with: ");
    // Serial.println(fillValues[i], HEX);
    fillVRAM((uint8_t) fillValues[i]);
    asmWait(65535, 50);
    checksum = readVRAM(true);
    valid = compareVRAMChecksum(checksum);
    if (!valid) {
      printLine(F("VRAM checksum did not match."));
      retVal = false;
      break;
    } else {
      printLine(F("VRAM checksum matched."));
    }
  }

  return retVal;
}

// Function to compare a passed checksum against the checksum table
bool compareVRAMChecksum(uint32_t checksum) {
  int checksumCounts = sizeof(vramChecksumTable) / sizeof(vramChecksumTable[0]);
  for (int i = 0; i < checksumCounts; i++) {
      if (vramChecksumTable[i] == checksum) {
          return true;
      }
  }
  return false;
}

/*
  Read byte from VRAM
  - Arduino address lines should be set for output
  - Arduino data lines should be set for input
*/
uint8_t readByteVRAM(uint16_t memAddress) {
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

/*
  Write byte to VRAM
  - Arduino address lines should be set for output
  - Arduino data lines should be set for output
*/
void writeByteVRAM(uint16_t memAddress, uint8_t data) {

  uint8_t lowerByte = (uint8_t)(memAddress & 0xFF); // Masking to get the lower byte
  uint8_t upperByte = (uint8_t)(memAddress >> 8);   // Shifting to get the upper byte

  PORTA = lowerByte;    // Write the lower byte of the address to PORTA
  PORTC = upperByte;    // Write the upper byte of the address to PORTC

  // RAS*
  pinMode(RAS_L, OUTPUT);
  digitalWrite(RAS_L, LOW);
  asmWait(1);

  // put data
  PORTF = data;

  // WR*
  pinMode(WR_L, OUTPUT);
  digitalWrite(WR_L, LOW);
  asmWait(3);

  // prep for exit
  turnOffReadWriteRASLines();
}

bool lowercaseModExists() {
  bool retVal = true;
  printLine(F("Checking for lowercase mod - requires additional SRAM chip and correct character ROM"));

  // TODO: rewrite
  // 1 - blank out the screen and verify checksum
  // 2 - write BFh to the screen and verify checksum
  uint8_t fillValues[] = {0x20, 0xBF};
  retVal = checkVRAMFillWithChecksum(fillValues, 2);

  if (!retVal) {
    Serial.println(F("Unable to clear VRAM, please run VRAM diagnostics to troubleshoot."));
    return retVal;
  }

  // at this point VRAM should be good, now check if bit 6 is on or off for these
  // If no lowercase mod: 6F -> 2F, 7F -> 3F, FF -> BF
  uint8_t fillValuesLC[] = {0x6F, 0x7F, 0xFF};

  retVal = checkVRAMFillWithChecksum(fillValuesLC, 3);

  return retVal;
}
