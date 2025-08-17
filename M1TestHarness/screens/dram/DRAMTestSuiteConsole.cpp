#include "./DRAMTestSuiteConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./DRAMMenu.h"

DRAMTestSuiteConsole::DRAMTestSuiteConsole() : RAMTestSuiteConsole() {
  setTitleF(F("DRAM Tests"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  // Set button labels
  const __FlashStringHelper *buttons[] = {F("M:Menu")};
  setButtonItemsF(buttons, 1);

  Globals.logger.infoF(F("DRAM Test Suite initialized"));
}

void DRAMTestSuiteConsole::_executeOnce() {
  cls();
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("=== DRAM TEST SUITE ==="));
  println();
  
  // Get current DRAM size from globals
  uint16_t dramSizeKB = Globals.getDRAMSizeKB();
  
  setTextColor(0xFFFF, 0x0000);
  print(F("Testing Dynamic RAM ("));
  print(dramSizeKB);
  println(F("KB)"));
  print(F("Memory Range: 0x4000-0x"));
  println(0x4000 + (dramSizeKB * 1024) - 1, HEX);
  println(F("IC References: Z17,Z16,Z18,Z19,Z15,Z20,Z14,Z13"));
  println();

  setTextColor(0xF81F, 0x0000);  // Magenta
  println(F("Starting DRAM comprehensive test..."));
  println();

  delay(5000);

  cls();
  setTextColor(0xFFFF, 0x0000);  // White

  // Local DRAM constants - use selected DRAM size
  const uint16_t start = 0x4000;                    // DRAM start address
  const uint16_t length = dramSizeKB * 1024;        // Selected DRAM size in bytes
  const char *const icRefs[] PROGMEM = {"Z17", "Z16", "Z18", "Z19", "Z15", "Z20", "Z14", "Z13"};

  // Run the comprehensive test suite on DRAM
  runSpecializedTest(start, length, icRefs);
}

Screen *DRAMTestSuiteConsole::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_MENU) {
    Globals.logger.infoF(F("Returning to main menu from DRAM Tests"));
    return new DRAMMenu();
  }

  return nullptr;
}
