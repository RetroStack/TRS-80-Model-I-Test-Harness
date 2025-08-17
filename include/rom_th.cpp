#include "rom_th.h"

#include "utils_th.h"  // Ensure extern parameters is visible

namespace RomTH {

void showMenu() {
  println(TO_LCD, F("a) Show ROM start address"));
  println(TO_LCD, F("b) Byte read: r,xxxx"));
  println(TO_LCD, F("c) Checksums for ROMs"));
  println(TO_LCD, F("d) Dump ROM to serial port"));
  println(TO_LCD, F("l) Show ROM length"));
  println(TO_LCD, F("v) Version"));
  println(TO_LCD, F("x) Back to main menu"));
  println(TO_LCD, F("B) Bus pin statuses"));
  println(TO_LCD, F("C) Clear screen"));
  // printTestSignalState();
  println(TO_LCD, F("X) Reset Test Harness"));
  println(TO_LCD, F("?  help"));
}

void handleInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(TO_LCD, F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case 'a':
      getROMStartAddress();
      break;
    case 'b':
      bytesRead();
      break;
    case 'c':
      checksumsForROMs();
      break;
    case 'd':
      dumpROM();
      break;
    case 'l':
      getROMLength();
      break;
    case 'v':
      version();
      break;
    case 'x':
      Menu::pop();
      break;
    case '?': /* Menu system auto-shows */
      break;
    default:
      println(TO_LCD, F("Invalid input"));
      break;
  }
}

void getROMStartAddress() {
  println(TO_LCD, F("Which ROMs (0-3)?  Separate by commas or 'a' for all:"));
  print(TO_LCD, F("> "));
  serialFlush();

  /* ---- parse input line ---- */
  char* params[MAX_INPUT_PARAMETERS];  // local bucket
  uint8_t paramCount = readSerialInputParse(params, MAX_INPUT_PARAMETERS);
  serialFlush();

  /* ---- "a" or "A" means all ROMs ---- */
  if (paramCount > 0 && (params[0][0] == 'a' || params[0][0] == 'A')) {
    for (uint8_t romNum = 0; romNum < 4; ++romNum) {
      uint16_t addr = rom.getROMStartAddress(romNum);
      println(TO_LCD, F("ROM "), romNum, F(" Start Address: "), addr, Hex);
    }
    return;
  }

  /* ---- otherwise process each comma-separated token ---- */
  for (uint8_t i = 0; i < paramCount; ++i) {
    if (params[i] == nullptr || params[i][0] == '\0')  // empty field
      continue;

    uint16_t romNum = strToUint16(params[i]);  // 0-3 expected
    if (romNum < 4) {
      uint16_t addr = rom.getROMStartAddress((uint8_t)romNum);
      println(TO_LCD, F("ROM "), romNum, F(" Start Address: "), addr, Hex);
    } else {
      println(TO_LCD, F("Invalid ROM number: "), params[i]);
    }
  }
}

// Get ROM length for specified ROMs
// Get ROM length for specified ROMs
void getROMLength() {
  println(TO_LCD, F("Enter ROM numbers for length (0-3), comma-separated, or 'a' for all:"));
  print(TO_LCD, F("> "));
  serialFlush();

  /* ---- parse input ---- */
  char* tokens[MAX_INPUT_PARAMETERS];  // local token list
  uint8_t tokCount = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);
  serialFlush();
  if (tokCount == 0)
    return;  // user hit <Enter> only

  /* ---- "a" / "A"  ->  all ROMs ---- */
  if (tolower(tokens[0][0]) == 'a') {
    for (uint8_t romNum = 0; romNum < 4; ++romNum) {
      uint16_t len = rom.getROMLength(romNum);
      println(TO_LCD, F("ROM "), romNum, F(" Length: "), len, Dec);
    }
    return;
  }

  /* ---- otherwise, handle each comma-separated token ---- */
  for (uint8_t i = 0; i < tokCount; ++i) {
    if (tokens[i] == nullptr || tokens[i][0] == '\0')
      continue;  // empty field

    uint16_t romNum = strToUint16(tokens[i]);  // 0-3 expected
    if (romNum < 4) {
      uint16_t len = rom.getROMLength((uint8_t)romNum);
      println(TO_LCD, F("ROM "), romNum, F(" Length: "), len, Dec);
    } else {
      println(TO_LCD, F("Invalid ROM number: "), tokens[i]);
    }
  }
}

// Read bytes from ROM
// TODO: the can be collapsed with similiar routine in ram_th.cpp
void bytesRead() {
  println(TO_LCD, F("[ROM] Byte Read"));
  println(TO_LCD, F("Enter start address, bytes to read (comma separated):"));
  print(TO_LCD, F("> "));
  serialFlush();

  /* ---- get "start, length" ---- */
  char* tokens[MAX_INPUT_PARAMETERS];
  uint8_t nTok = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);
  serialFlush();

  /* ---- need exactly two tokens ---- */
  if (nTok < 2 || tokens[0][0] == '\0' || tokens[1][0] == '\0') {
    println(TO_LCD, F("Error: enter two numbers (dec/hex/bin) separated by a comma."));
    return;
  }

  uint16_t start = strToUint16(tokens[0]);
  uint16_t length = strToUint16(tokens[1]);

  println(TO_LCD, F("Start: "), start, Hex);
  println(TO_LCD, F("Length: "), length, Hex);

  /* ---- perform byte-read operation ---- */
  println(TO_LCD, F("[ROM] Reading bytes from ROM..."));
  Model1.printMemoryContents(logger, start, length,
                             BOTH,   // display mode
                             false,  // no ASCII?
                             16);    // bytes per line
}

void checksumsForROMs() {
  println(TO_LCD, F("[ROM] Checksums for ROMs"));
  for (uint8_t romNum = 0; romNum < 4; ++romNum) {
    uint16_t checksum = rom.getChecksum(romNum);
    println(TO_LCD, F("ROM "), romNum, F(": "), checksum, Hex);
  }
}

// Dump selected ROMs (0-3) or all ("a")
void dumpROM() {
  println(TO_LCD, F("[ROM] Dump ROM"));
  println(TO_LCD, F("Enter ROM numbers to dump (0-3), comma-separated, or 'a' for all:"));
  print(TO_LCD, F("> "));
  serialFlush();

  /* ---- parse input line ---- */
  char* tokens[MAX_INPUT_PARAMETERS];
  uint8_t tokCount = readSerialInputParse(tokens, MAX_INPUT_PARAMETERS);
  serialFlush();

  bool validInput = false;

  /* ---- "a" / "A" -> dump all ROMs ---- */
  if (tokCount > 0 && (tokens[0][0] == 'a' || tokens[0][0] == 'A')) {
    validInput = true;
    for (uint8_t romNum = 0; romNum < 4; ++romNum) {
      println(TO_LCD, F("--- ROM "), romNum, F(" ---"));
      rom.printROMContents(romNum);
    }
  }
  /* ---- otherwise, handle each comma-separated token ---- */
  else {
    for (uint8_t i = 0; i < tokCount; ++i) {
      if (tokens[i] == nullptr || tokens[i][0] == '\0')
        continue;  // empty field

      uint16_t romNum = strToUint16(tokens[i]);
      if (romNum < 4) {
        validInput = true;
        println(TO_LCD, F("--- ROM "), romNum, F(" ---"));
        rom.printROMContents((uint8_t)romNum);
      } else {
        println(TO_LCD, F("Invalid ROM number: "), tokens[i]);
      }
    }
  }

  if (!validInput)
    println(TO_LCD, F("No valid ROMs selected.  Please enter 0-3 or 'a' for all."));

  serialFlush();
  inputPrompt("Press ? for menu.");
}

void version() {
  // println(TO_LCD, F("[ROM] Version"));
  const __FlashStringHelper* systemName = rom.identifyROM();
  // println(systemName);
  println(TO_LCD, F("[ROM] Version "), systemName);
}

}  // namespace RomTH
