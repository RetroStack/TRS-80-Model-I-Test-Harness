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
  setTextColor(0xFFFF, 0x0000);
  println(F("Testing Dynamic RAM (16KB)"));
  println(F("Memory Range: 0x4000-0x7FFF"));
  println(F("IC References: Z17,Z16,Z18,Z19,Z15,Z20,Z14,Z13"));
  println();

  setTextColor(0xF81F, 0x0000);  // Magenta
  println(F("Starting DRAM comprehensive test..."));
  println();

  delay(5000);

  cls();
  setTextColor(0xFFFF, 0x0000);  // White

  // Local DRAM constants - only exist when test is running
  const uint16_t start = 0x4000;      // DRAM start address
  const uint16_t length = 1024 * 16;  // 16KB DRAM
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
