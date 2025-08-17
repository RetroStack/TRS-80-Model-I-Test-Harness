#include "ram_th.h"

namespace RamTH {

// DRAM
uint16_t dramStart = 0x4000;
uint16_t dramLength = 1024 * 16;

const char *dramICs[8] = {"Z17", "Z16", "Z18", "Z19", "Z15", "Z20", "Z14", "Z13"};
const char *vramICs[8] = {"Z48", "Z47", "Z46", "Z45", "Z61", "Z62", "Z?", "Z63"};

// VRAM
uint16_t vramStart = 0x3C00;
uint16_t vramLength = 1024;

// Show the main menu
void showMenu() {
  println(TO_LCD, F("b) Byte read|write"));
  println(TO_LCD, F("f) Fill memory - byte"));
  println(TO_LCD, F("F) Fill memory - pattern"));
  println(TO_LCD, F("g) Game upload (alninvbh)"));
  println(TO_LCD, F("r) Memory -> serial port (HEX)"));
  println(TO_LCD, F("R) Memory -> serial port (ASCII)"));
  println(TO_LCD, F("t) TESTS ->"));
  println(TO_LCD, F("u) Upload data from serial port [TODO]"));
  println(TO_LCD, F("x) Back to main menu"));
  println(TO_LCD, F("B) Bus pin statuses"));
  println(TO_LCD, F("C) Clear screen"));
  // TODO: enable once memory bug fixed. printTestSignalState();
  println(TO_LCD, F("X) Reset Test Harness"));
  println(TO_LCD, F("?  help"));
}

// Show the tests menu
void showTestsMenu() {
  println(TO_LCD, F(" 1) Address Uniqueness (55 Pattern)"));
  println(TO_LCD, F(" 2) Address Uniqueness (AA Pattern)"));
  println(TO_LCD, F(" 3) Checkerboard (Inverted)"));
  println(TO_LCD, F(" 4) Checkerboard (Normal)"));
  println(TO_LCD, F(" 5) March C Algorithm"));
  println(TO_LCD, F(" 6) March LA Algorithm"));
  println(TO_LCD, F(" 7) March SS Algorithm"));
  println(TO_LCD, F(" 8) Moving Inversion (55 Pattern)"));
  println(TO_LCD, F(" 9) Moving Inversion (Random Pattern)"));
  println(TO_LCD, F("10) Moving Inversion (Zero Pattern)"));
  println(TO_LCD, F("11) Read Destructive (55 Pattern)"));
  println(TO_LCD, F("12) Read Destructive (AA Pattern)"));
  println(TO_LCD, F("13) Repeated Read (Inverted)"));
  println(TO_LCD, F("14) Repeated Read (Normal)"));
  println(TO_LCD, F("15) Repeated Write (Inverted)"));
  println(TO_LCD, F("16) Repeated Write (Normal)"));
  println(TO_LCD, F("17) Retention Test"));
  println(TO_LCD, F("18) Walking Ones Test"));
  println(TO_LCD, F("19) Walking Zeros Test"));
  println(TO_LCD, F(" x) Back"));
  println(TO_LCD, F(" ?  Help"));
  print(TO_LCD, F("> "));
}

// Main entry point for RAM tests
void runTest(int testNum) {
  bool aTestExecuted = true;
  TestResult testResult = {0};
  char *tokens[MAX_INPUT_PARAMETERS];

  println(TO_LCD, F("Enter: [1 = DRAM | 2 = VRAM | 3 = Anywhere],bytes to test:"));
  print(TO_LCD, F("> "));
  serialFlush();
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);

  /* ---- need exactly two tokens ---- */
  if (nTok < 2 || tokens[0][0] == '\0' || tokens[1][0] == '\0') {
    println(TO_LCD, F("Error: enter two numbers (dec/hex/bin) separated by a comma."));
    return;
  }

  uint8_t memoryLocType = strToUint16(tokens[0]);
  uint16_t start = 0;
  uint16_t length = strToUint16(tokens[1]);

  if (memoryLocType == 1) {
    start = dramStart;
    println(TO_LCD, F("Testing DRAM..."));
  } else if (memoryLocType == 2) {
    start = vramStart;
    println(TO_LCD, F("Testing VRAM..."));
  } else if (memoryLocType == 3) {
    start = 0;  // Anywhere
    println(TO_LCD, F("Testing anywhere..."));
    // get start address from user input
    println(TO_LCD, F("Starting address? "));
    nTok = readSerialInputParse(tokens, 1);
    if (nTok > 0 && tokens[0][0] != '\0') {
      start = strToUint16(tokens[0]);
      println(TO_LCD, F("Starting address: "), start, Hex);
    } else {
      println(TO_LCD, F("Error: invalid starting address."));
      return;
    }
  } else {
    println(TO_LCD, F("Error: first parameter must be 1 (DRAM) or 2 (VRAM) or 3 (anywhere)."));
    return;
  }

  if (length == 0) {
    println(TO_LCD, F("Error: length can't be 0."));
    return;
  }

  if ((memoryLocType == 1) && (length > dramLength)) {
    println(TO_LCD, F("Error: length exceeds DRAM limits."));
    return;
  }

  if ((memoryLocType == 2) && (length > vramLength)) {
    println(TO_LCD, F("Error: length exceeds VRAM limits."));
    return;
  }

  if ((memoryLocType == 3) && (start + length > 0xFFFF)) {
    println(TO_LCD, F("Error: length exceeds memory limits."));
    return;
  }

  // Show details
  print(TO_LCD, F("Start: "), start, Hex);
  println(TO_LCD, F("h"));
  print(TO_LCD, F("Length: "), length, Hex);
  println(TO_LCD, F("h"));

  switch (testNum) {
    case 1: {
      printSeparator(TO_LCD, F("[RAM] Address Uniqueness (55 Pattern)"), '-', 52, 0, 5);
      testResult = runAddressUniquenessTest(start, length, 0x55);
      break;
    }
    case 2: {
      printSeparator(TO_LCD, F("[RAM] Address Uniqueness (AA Pattern)"), '-', 52, 0, 5);
      testResult = runAddressUniquenessTest(start, length, 0xAA);
      break;
    }
    case 3: {
      printSeparator(TO_LCD, F("[RAM] Checkerboard (Inverted)"), '-', 52, 0, 5);
      testResult = runCheckerboardTest(start, length, false);
      break;
    }
    case 4: {
      printSeparator(TO_LCD, F("[RAM] Checkerboard (Normal)"), '-', 52, 0, 5);
      testResult = runCheckerboardTest(start, length, true);
      break;
    }
    case 5: {
      printSeparator(TO_LCD, F("[RAM] March C Algorithm"), '-', 52, 0, 5);
      testResult = runMarchCTest(start, length);
      break;
    }
    case 6: {
      printSeparator(TO_LCD, F("[RAM] March LA Algorithm"), '-', 52, 0, 5);
      testResult = runMarchLATest(start, length);
      break;
    }
    case 7: {
      printSeparator(TO_LCD, F("[RAM] March SS Algorithm"), '-', 52, 0, 5);
      testResult = runMarchSSTest(start, length);
      break;
    }
    case 8: {
      printSeparator(TO_LCD, F("[RAM] Moving Inversion (55 Pattern)"), '-', 52, 0, 5);
      testResult = runMovingInversionTest(start, length, 0x55);
      break;
    }
    case 9: {
      printSeparator(TO_LCD, F("[RAM] Moving Inversion (Random Pattern)"), '-', 52, 0, 5);
      testResult = runMovingInversionTest(start, length, random(0, 255));
      break;
    }
    case 10: {
      printSeparator(TO_LCD, F("[RAM] Moving Inversion (Zero Pattern)"), '-', 52, 0, 5);
      testResult = runMovingInversionTest(start, length, 0x00);
      break;
    }
    case 11: {
      printSeparator(TO_LCD, F("[RAM] Read Destructive (55 Pattern)"), '-', 52, 0, 5);
      testResult = runReadDestructiveTest(start, length, 0x55, 5);
      break;
    }
    case 12: {
      printSeparator(TO_LCD, F("[RAM] Read Destructive (AA Pattern)"), '-', 52, 0, 5);
      testResult = runReadDestructiveTest(start, length, 0xAA, 5);
      break;
    }
    case 13: {
      printSeparator(TO_LCD, F("[RAM] Repeated Read (Inverted)"), '-', 52, 0, 5);
      testResult = runRepeatedReadTest(start, length, false);
      break;
    }
    case 14: {
      printSeparator(TO_LCD, F("[RAM] Repeated Read (Normal)"), '-', 52, 0, 5);
      testResult = runRepeatedReadTest(start, length, true);
      break;
    }
    case 15: {
      printSeparator(TO_LCD, F("[RAM] Repeated Write (Inverted)"), '-', 52, 0, 5);
      testResult = runRepeatedWriteTest(start, length, false);
      break;
    }
    case 16: {
      printSeparator(TO_LCD, F("[RAM] Repeated Write (Normal)"), '-', 52, 0, 5);
      testResult = runRepeatedWriteTest(start, length, true);
      break;
    }
    case 17: {
      printSeparator(TO_LCD, F("[RAM] Retention Test"), '-', 52, 0, 5);
      testResult = runRetentionTest(start, length, 0xFF, 1000, 5);
      break;
    }
    case 18: {
      printSeparator(TO_LCD, F("[RAM] Running Walking Ones Test"), '-', 52, 0, 5);
      testResult = runWalkingOnesTest(start, length);
      break;
    }
    case 19: {
      printSeparator(TO_LCD, F("[RAM] Walking Zeros Test"), '-', 52, 0, 5);
      testResult = runWalkingZerosTest(start, length);
      break;
    }
    default:
      println(TO_LCD, F("Invalid test number"));
      aTestExecuted = false;
      break;
  }

  if (aTestExecuted) {
    println(TO_LCD, F("Test completed successfully."));
    println(TO_LCD, F("Total Errors: "), testResult.totalErrors);

    if (memoryLocType == 1) {
      printResult("DRAM Test Result", testResult, dramICs);
    } else if (memoryLocType == 2) {
      printResult("VRAM Test Result", testResult, vramICs);
    } else {
      println(TO_LCD, F("Anywhere Test Result"));
      for (uint8_t b = 0; b < 8; b++) {
        println(TO_LCD, F("  Bit "), b, F(" Errors: "), testResult.bitErrors[b]);
      }
    }
  } else {
    println(TO_LCD, F("No test executed."));
  }

  inputPrompt(TO_LCD, F("Press ? for menu."));
  serialFlush();
}

// Handle input for tests menu
void handleTestsMenuInput(const char *input) {
  if (isCommand(input, "x")) {
    Menu::pop();
    return;
  }

  if (isCommand(input, "?")) {
    // Menu system will auto-show current menu
    return;
  }

  int testNum = atoi(input);
  if (testNum >= 1 && testNum <= 19) {
    runTest(testNum);
  } else {
    println(TO_LCD, F("Invalid input, ? for help."));
  }
}

// Handle input for main menu
void handleMainMenuInput(const char *input) {
  if (strlen(input) != 1) {
    println(TO_LCD, F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case '?': /* Menu system auto-shows */
      break;
    case 'b':
      bytesReadWrite();
      break;
    case 'c':
      copyMemory();
      break;
    case 'f':
      fillMemoryByte();
      break;
    case 'F':
      fillMemoryPattern();
    case 'g':
      gameUpload();
      break;
    case 'r':
      streamMemory(true);
      break;
    case 'R':
      streamMemory(false);
      break;
    case 't':
      Menu::push(Menu::NodeID::RAM_TESTS);
      break;
    case 'u':
      uploadData();
      break;
    case 'x':
      Menu::pop();
      break;
    default:
      println(TO_LCD, F("Invalid input"));
      break;
  }
}

// Controller to decide if to handle input for tests or main menu
void handleInput(const char *input) {
  if (!input || strlen(input) == 0) {
    println(TO_LCD, F("Invalid input"));
    return;
  }

  // Handle based on current menu context
  switch (Menu::current()) {
    case Menu::NodeID::RAM_TESTS:
      handleTestsMenuInput(input);
      break;
    case Menu::NodeID::RAM:
      handleMainMenuInput(input);
      break;
    default:
      println(TO_LCD, F("Unknown menu state"));
      break;
  }
}

// copy memory from one address to another
void copyMemory() {
  println(TO_LCD, F("Enter: [start address, end address, bytes to copy]:"));
  print(TO_LCD, F("> "));
  serialFlush();

  char *tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);

  // Need exactly three tokens: start, end, and length
  if (nTok != 3 || tokens[0][0] == '\0' || tokens[1][0] == '\0' || tokens[2][0] == '\0') {
    println(TO_LCD, F("*ERR* Format: start,end,length (e.g. 3c00h,3cffh,100)"));
    return;
  }

  uint16_t src_address = strToUint16(tokens[0]);
  uint16_t dst_address = strToUint16(tokens[1]);
  uint16_t bytes_to_copy = strToUint16(tokens[2]);

  printSeparator(TO_LCD, F("[RAM] Copying memory"), '-', 30, 0);
  // debug info
  println(TO_LCD, F("Start: "), src_address, Hex);
  println(TO_LCD, F("End: "), dst_address, Hex);
  println(TO_LCD, F("Bytes to copy: "), bytes_to_copy);

  if (src_address >= dst_address) {
    println(TO_LCD, F("Error: start address must be less than end address."));
    return;
  }

  Model1.copyMemory(src_address, dst_address, bytes_to_copy);
}

// Read bytes from RAM (anywhere where really) - TODO: possible merge with ROM version
void bytesReadWrite() {
  println(TO_LCD, F("[RAM] Byte Read/Write"));
  println(TO_LCD, F("Enter: [R|W], start address, [bytes to read | write data in hex]:"));
  print(TO_LCD, F("> "));
  serialFlush();

  /* ---- get "start, length" ---- */
  char *tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);
  serialFlush();

  // // debug to print out tokens
  // for (uint8_t i = 0; i < nTok; i++)
  //   println(TO_LCD, F("Token "), i, F(": "), tokens[i]);

  /* ---- need exactly three tokens ---- */
  if (nTok < 3 || tokens[0][0] == '\0' || tokens[1][0] == '\0' || tokens[2][0] == '\0') {
    println(TO_LCD,
            F("Error: enter three values (R|W, start, [length | data]) separated by commas."));
    return;
  }

  // Check if first token is R (read) or W (write)
  bool isWrite = (tokens[0][0] == 'W' || tokens[0][0] == 'w');
  if (!isWrite && (tokens[0][0] != 'R' && tokens[0][0] != 'r')) {
    println(TO_LCD, F("Error: first parameter must be R (read) or W (write)."));
    return;
  }

  // 2nd and 3rd tokens are start address and length or data
  uint16_t start = strToUint16(tokens[1]);
  uint16_t length = strToUint16(tokens[2]);
  uint8_t dataLength = strlen(tokens[2]);

  // confirm operation
  if (isWrite) {
    println(TO_LCD, F("Writing data to RAM..."));
    println(TO_LCD, F("Start: "), start, Hex, 'h');
    println(TO_LCD, F("Data: "), tokens[2]);
    if (dataLength & 1) {
      println(TO_LCD, F("Error: data string must be even number of characters."));
      return;
    }
  } else {
    println(TO_LCD, F("Reading data from RAM..."));
    println(TO_LCD, F("Start: "), start, Hex, 'h');
    println(TO_LCD, F("Length: "), length, Hex, 'h');
  }

  if (!isWrite) {
    /* ---- perform byte-read operation ---- */
    println(TO_LCD, F("[RAM] Reading bytes from RAM..."));
    Model1.printMemoryContents(logger, start, length,
                               BOTH,   // display mode
                               false,  // relative?
                               16);    // bytes per line
  } else {
    /* ---- perform byte-write operation ---- */
    println(TO_LCD, F("[RAM] Writing bytes to RAM..."));
    // Convert the data string to uint8_t, every two characters is a byte
    // tokens[2] is the data string

    char hex[3];
    hex[2] = '\0';  // Null-terminate the string
    for (uint16_t i = 0; i < dataLength; i += 2) {
      hex[0] = tokens[2][i];
      hex[1] = tokens[2][i + 1];
      uint8_t value = (uint8_t)strToUint16(hex);
      // debug print of memory address and value
      println(TO_LCD, F("Writing to address: "), start + (i >> 1), Hex);
      println(TO_LCD, F("Value: "), value, 'h');

      Model1.writeMemory(start + (i >> 1), value);
    }

    println(TO_LCD, F("Bytes written successfully."));
  }
}

// Fills memory with a byte value
void fillMemoryByte() {
  println(TO_LCD, F("Enter [start address, length, byte value]:"));
  print(TO_LCD, F("> "));
  serialFlush();
  char *tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);

  /* ---- need exactly two tokens ---- */
  if (nTok < 3 || tokens[0][0] == '\0' || tokens[1][0] == '\0') {
    println(TO_LCD, F("Error: enter start address and length separated by commas."));
    return;
  }

  uint16_t address = strToUint16(tokens[0]);
  uint16_t length = strToUint16(tokens[1]);
  uint8_t data = (uint8_t)strToUint16(tokens[2]);

  // Fill memory with byte value
  printSeparator(TO_LCD, F("[RAM] Filling memory with byte value"), '-', 30, 0);
  Model1.fillMemory(data, address, length);
}

// Fills memory with a pattern
void fillMemoryPattern() {
  println(TO_LCD, F("Enter [start address, end address, pattern]:"));
  print(TO_LCD, F("> "));
  serialFlush();

  char *tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);

  // Need exactly three tokens: start, end, pattern
  if (nTok != 3 || tokens[0][0] == '\0' || tokens[1][0] == '\0' || tokens[2][0] == '\0') {
    println(TO_LCD, F("*ERR* Format: start,end,pattern  (e.g. 3c00h,3cffh,515253)"));
    return;
  }

  // debug print tokens
  println(TO_LCD, F("Start: "), tokens[0]);
  println(TO_LCD, F("End: "), tokens[1]);
  println(TO_LCD, F("Pattern: "), tokens[2]);

  uint16_t start_address = strToUint16(tokens[0]);
  uint16_t end_address = strToUint16(tokens[1]);

  if (start_address > end_address) {
    println(TO_LCD, F("*ERR* Start address greater than end address."));
    return;
  }

  /* ---------- Parse the pattern ---------- */
  const char *pat = tokens[2];
  size_t patLen = strlen(pat);

  if (patLen & 1) {  // odd number of nybbles
    println(TO_LCD, F("*ERR* Pattern must contain an even # of hex digits."));
    return;
  }

  uint8_t fill_data[64];  // 64-byte max pattern (adjust if needed)
  uint8_t fill_data_len = 0;

  for (size_t i = 0; i < patLen && fill_data_len < sizeof(fill_data); i += 2) {
    char byteHex[3] = {pat[i], pat[i + 1], '\0'};
    fill_data[fill_data_len++] = (uint8_t)strtol(byteHex, nullptr, 16);
  }

  if (fill_data_len == 0) {
    println(TO_LCD, F("*ERR* Empty pattern."));
    return;
  }

  /* ---------- Fill memory ---------- */
  printSeparator(TO_LCD, F("[RAM] Filling memory"), '-', 30, 0);

  // Option A - your existing helper repeats the pattern internally:
  Model1.fillMemory(fill_data, fill_data_len, start_address, end_address);

  // manual loop
  uint16_t span = end_address - start_address + 1;
  for (uint16_t i = 0; i < span; ++i) {
    uint8_t b = fill_data[i % fill_data_len];
    Model1.writeMemory(start_address + i, b);
  }

  println(TO_LCD, F("Memory filled successfully."));
}

void streamMemory(bool inHex) {
  println(TO_LCD, F("Enter start address and length (e.g., 0x4000, 16):"));
  print(TO_LCD, F("> "));
  serialFlush();
  char *tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);
  /* ---- need exactly two tokens ---- */
  if (nTok < 2 || tokens[0][0] == '\0' || tokens[1][0] == '\0') {
    println(TO_LCD, F("Error: enter start address and length separated by commas."));
    return;
  }

  uint16_t start = strToUint16(tokens[0]);
  uint16_t length = strToUint16(tokens[1]);

  if (length == 0) {
    println(TO_LCD, F("Error: length can't be 0."));
    return;
  }

  // Start streaming memory
  printSeparator(TO_LCD, F("[RAM] Streaming memory"), '-', 30, 0);
  for (uint16_t i = 0; i < length; i++) {
    uint8_t data = Model1.readMemory(start + i);
    if (inHex) {
      Serial.print(data, HEX);
    } else {
      Serial.print(data, ASCII);
    }
  }
  Serial.println();
  printSeparator(TO_LCD, F("End of memory stream"), '-', 30, 0);
}

void gameUpload() {
  uint16_t start_addr = 0x5200;
  uint16_t data_size = sizeof(alninvbh_code);
  uint16_t end_addr = start_addr + data_size - 1;
  Model1.fillMemory((uint8_t *)alninvbh_code, data_size, start_addr, end_addr);
}

void readHex() {
  println(TO_LCD, F("[RAM] readHex() - TODO"));
}
void readAscii() {
  println(TO_LCD, F("[RAM] readAscii() - TODO"));
}
void uploadData() {
  println(TO_LCD, F("[RAM] uploadData() - TODO"));
}

#define INIT_TEST_RESULT TestResult result = {}
#define UPDATE_ERRORS(diff)           \
  if (diff != 0) {                    \
    result.totalErrors++;             \
    for (uint8_t b = 0; b < 8; b++) { \
      if (diff & (1 << b))            \
        result.bitErrors[b]++;        \
    }                                 \
  }

TestResult runRepeatedWriteTest(uint16_t start, uint16_t length, bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Repeated Write Test");

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    for (uint16_t j = 0; j < 5; j++) {
      Model1.writeMemory(start + i, 0x55);
    }
  }
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x55;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runRepeatedReadTest(uint16_t start, uint16_t length, bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Repeated Read Test");

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x55);
  }
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    for (uint16_t j = 0; j < 5; j++) {
      data = Model1.readMemory(start + i);
    }
    uint8_t diff = data ^ 0x55;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runCheckerboardTest(uint16_t start, uint16_t length, bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Checkerboard Test");
  if (toggleStart) {
    Serial.print("(normal)");
  } else {
    Serial.print("(inverted)");
  }

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    bool toggle = (i & 1) ? !toggleStart : toggleStart;
    if (toggle) {
      Model1.writeMemory(start + i, 0x55);
    } else {
      Model1.writeMemory(start + i, 0xAA);
    }
  }
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    bool toggle = (i & 1) ? !toggleStart : toggleStart;
    data = Model1.readMemory(start + i);
    if (toggle) {
      uint8_t diff = data ^ 0x55;
      UPDATE_ERRORS(diff);
    } else {
      uint8_t diff = data ^ 0xAA;
      UPDATE_ERRORS(diff);
    }
  }
  Serial.println();

  return result;
}

TestResult runWalkingOnesTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Walking Ones Test");

  for (uint8_t bit = 0; bit < 8; bit++) {
    uint8_t pattern = 1 << bit;
    Serial.print(".");

    // Write
    for (uint16_t i = 0; i < length; i++) {
      Model1.writeMemory(start + i, pattern);
    }

    // Read
    for (uint16_t i = 0; i < length; i++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
    }
  }
  Serial.println();

  return result;
}

TestResult runWalkingZerosTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Walking Zeros Test");

  for (uint8_t bit = 0; bit < 8; bit++) {
    uint8_t pattern = ~(1 << bit);
    Serial.print(".");

    // Write
    for (uint16_t i = 0; i < length; i++) {
      Model1.writeMemory(start + i, pattern);
    }

    // Read
    for (uint16_t i = 0; i < length; i++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
    }
  }
  Serial.println();

  return result;
}

TestResult runMarchCTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("March C- Test");

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  Serial.print(".");
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  Serial.print(".");
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runMovingInversionTest(uint16_t start, uint16_t length, uint8_t pattern) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Moving Inversion Test (0x");
  Serial.print(pattern, HEX);
  Serial.print(")");

  // First pattern: 0x00
  uint8_t antipattern = ~pattern;

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, antipattern);
  }

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ antipattern;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, pattern);
  }

  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runRetentionTest(uint16_t start, uint16_t length, uint8_t pattern, uint32_t delayMs,
                            uint8_t repeatDelay) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Retention Test (0x");
  Serial.print(pattern, HEX);
  Serial.print(")");

  // Fill with pattern
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  for (uint16_t i = 0; i < repeatDelay; i++) {
    Serial.print(".");
    delay(delayMs);
  }
  // Verify
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runMarchSSTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("March SS Test");

  // Phase 1: Up write 0
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 2: Up read 0, write 1
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 3: Down read 1, write 0
  Serial.print(".");
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 4: Down read 0, write 1
  Serial.print(".");
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 5: Up read 1, write 0
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 6: Up read 0
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runMarchLATest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("March LA Test");

  // Phase 1: Up write 0
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 2: Up read 0, write 1
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 3: Down read 1, write 0
  Serial.print(".");
  for (int32_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 4: Down read 0
  Serial.print(".");
  for (int32_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestResult runReadDestructiveTest(uint16_t start, uint16_t length, uint8_t pattern,
                                  uint8_t numReads) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Read Destructive Fault Test (pattern 0x");
  Serial.print(pattern, HEX);
  Serial.print(", reads: ");
  Serial.print(numReads);
  Serial.print(")");

  // 1. Fill memory with pattern
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  // 2. Repeatedly read the same cell without re-writing
  for (uint16_t i = 0; i < length; i++) {
    for (uint8_t r = 0; r < numReads; r++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
      if (diff != 0) {
        break;
      }
    }
  }
  Serial.println();

  return result;
}

TestResult runAddressUniquenessTest(uint16_t start, uint16_t length, uint8_t pattern) {
  uint8_t data;
  INIT_TEST_RESULT;

  Serial.print("Address Uniqueness Test (XOR pattern 0x");
  Serial.print(pattern, HEX);
  Serial.print(")");

  // Phase 1: Write unique XOR pattern
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    uint8_t value = ((uint8_t)(i & 0xFF)) ^ pattern;
    Model1.writeMemory(start + i, value);
  }

  // Phase 2: Verify
  Serial.print(".");
  for (uint16_t i = 0; i < length; i++) {
    uint8_t expected = ((uint8_t)(i & 0xFF)) ^ pattern;
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ expected;
    UPDATE_ERRORS(diff);
  }
  Serial.println();

  return result;
}

TestSuiteResult runMemoryTestSuite(uint16_t start, uint16_t length) {
  TestSuiteResult suite = {};

  suite.repeatedWriteNormal = runRepeatedWriteTest(start, length, true);
  suite.repeatedWriteInverted = runRepeatedWriteTest(start, length, false);
  suite.repeatedReadNormal = runRepeatedReadTest(start, length, true);
  suite.repeatedReadInverted = runRepeatedReadTest(start, length, false);
  suite.checkerboardNormal = runCheckerboardTest(start, length, true);
  suite.checkerboardInverted = runCheckerboardTest(start, length, false);
  suite.walkingOnes = runWalkingOnesTest(start, length);
  suite.walkingZeros = runWalkingZerosTest(start, length);
  suite.marchC = runMarchCTest(start, length);
  suite.movingInversionZero = runMovingInversionTest(start, length, 0x00);
  suite.movingInversion55 = runMovingInversionTest(start, length, 0x55);
  suite.movingInversionRandom = runMovingInversionTest(start, length, random(0, 255));
  suite.marchSS = runMarchSSTest(start, length);
  suite.marchLA = runMarchLATest(start, length);
  suite.readDestructiveAA = runReadDestructiveTest(start, length, 0xAA, 5);
  suite.readDestructive55 = runReadDestructiveTest(start, length, 0x55, 5);
  suite.addressUniqueness55 = runAddressUniquenessTest(start, length, 0x55);
  suite.addressUniquenessAA = runAddressUniquenessTest(start, length, 0xAA);
  suite.retention = runRetentionTest(start, length, 0xFF, 1000, 5);

  return suite;
}

void printResult(const char *name, const TestResult &r, const char *icRefs[8]) {
  Serial.print(name);
  Serial.print(" - Total Errors: ");
  Serial.println(r.totalErrors);
  for (uint8_t b = 0; b < 8; b++) {
    Serial.print("  Bit ");
    Serial.print(b);
    Serial.print(" (");
    Serial.print(icRefs[b]);
    Serial.print("): ");
    Serial.println(r.bitErrors[b]);
  }
}

void runAndEvaluate(uint16_t start, uint16_t length, const char *icRefs[8]) {
  Serial.println("=== START MEMORY TEST SUITE ===");

  TestSuiteResult suite = runMemoryTestSuite(start, length);

  // Serial.println();
  // Serial.println("--- Per-Test Results ---");

  // printResult("Repeated Write Normal", suite.repeatedWriteNormal, icRefs);
  // printResult("Repeated Write Inverted", suite.repeatedWriteInverted, icRefs);
  // printResult("Repeated Read Normal", suite.repeatedReadNormal, icRefs);
  // printResult("Repeated Read Inverted", suite.repeatedReadInverted, icRefs);
  // printResult("Checkerboard Normal", suite.checkerboardNormal, icRefs);
  // printResult("Checkerboard Inverted", suite.checkerboardInverted, icRefs);
  // printResult("Walking Ones", suite.walkingOnes, icRefs);
  // printResult("Walking Zeros", suite.walkingZeros, icRefs);
  // printResult("March C-", suite.marchC, icRefs);
  // printResult("Moving Inversion 0x00", suite.movingInversionZero, icRefs);
  // printResult("Moving Inversion 0x55", suite.movingInversion55, icRefs);
  // printResult("Moving Inversion Random", suite.movingInversionRandom, icRefs);
  // printResult("March SS", suite.marchSS, icRefs);
  // printResult("March LA", suite.marchLA, icRefs);
  // printResult("Read Destructive 0xAA", suite.readDestructiveAA, icRefs);
  // printResult("Read Destructive 0x55", suite.readDestructive55, icRefs);
  // printResult("Address Uniqueness 0x55", suite.addressUniqueness55, icRefs);
  // printResult("Address Uniqueness 0xAA", suite.addressUniquenessAA, icRefs);
  // printResult("Retention", suite.retention, icRefs);

  Serial.println();
  Serial.println("--- Aggregate Summary ---");

  // 1) Initialize totals
  uint32_t totalBitErrors[8] = {};
  uint32_t totalErrorsOverall = 0;

  // 2) Create a pointer array to all TestResults for iteration
  const TestResult *allResults[] = {&suite.repeatedWriteNormal,
                                    &suite.repeatedWriteInverted,
                                    &suite.repeatedReadNormal,
                                    &suite.repeatedReadInverted,
                                    &suite.checkerboardNormal,
                                    &suite.checkerboardInverted,
                                    &suite.walkingOnes,
                                    &suite.walkingZeros,
                                    &suite.marchC,
                                    &suite.movingInversionZero,
                                    &suite.movingInversion55,
                                    &suite.movingInversionRandom,
                                    &suite.marchSS,
                                    &suite.marchLA,
                                    &suite.readDestructiveAA,
                                    &suite.readDestructive55,
                                    &suite.addressUniqueness55,
                                    &suite.addressUniquenessAA,
                                    &suite.retention};

  // 3) Sum all bit errors
  for (uint8_t t = 0; t < sizeof(allResults) / sizeof(allResults[0]); t++) {
    const TestResult *r = allResults[t];
    totalErrorsOverall += r->totalErrors;
    for (uint8_t b = 0; b < 8; b++) {
      totalBitErrors[b] += r->bitErrors[b];
    }
  }

  // 4) Print per-bit totals
  for (uint8_t b = 0; b < 8; b++) {
    Serial.print("Bit ");
    Serial.print(b);
    Serial.print(" (");
    Serial.print(icRefs[b]);
    Serial.print("): ");
    Serial.println(totalBitErrors[b]);
  }

  Serial.print("Total Errors Across All Tests: ");
  Serial.println(totalErrorsOverall);

  Serial.println("=== END MEMORY TEST SUITE ===");
}

void loopx() {
  delay(1000);

  Model1.writeMemory(0x3C00, 0x61);
  uint8_t data = Model1.readMemory(0x3C00);
  if (data == 0x21) {  // Shifted in page due to circuitry
    Serial.println("Most likely a Model 1 without lower-case mod.");
  } else {
    Serial.println("A model 1 with lower-case mod.");
  }

  // SAF - Stuck At Fault (SAF0 or SAF1) -> Cell always reads 0 or 1
  // TF - Transition Fault -> Cell cannot change state in one or both directions
  // AF - Address Fault -> Wrong cell is accessed or modified
  // CF - Coupling Fault
  // - id  = Inversion-Dominating Coupling Fault - Writing aggressor inverts victim
  // - den = Dominating Coupling Fault - Dynamic EnableWriting aggressor forces victim to fixed
  // value SCF - State Coupling Fault - Aggressor's state holds victim in state

  Serial.println("====================================");
  Serial.println("VRAM Tests");
  runAndEvaluate(vramStart, vramLength, vramICs);

  Serial.println("====================================");
  Serial.println("DRAM Tests");
  runAndEvaluate(dramStart, dramLength, dramICs);
}
///////////////////////////////

}  // namespace RamTH
