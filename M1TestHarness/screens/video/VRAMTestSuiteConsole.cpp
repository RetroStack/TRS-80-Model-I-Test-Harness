#include "./VRAMTestSuiteConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./VideoMenu.h"

VRAMTestSuiteConsole::VRAMTestSuiteConsole() : RAMTestSuiteConsole() {
  setTitleF(F("VRAM Tests"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  // Set button labels
  const __FlashStringHelper *buttons[] = {F("M:Menu")};
  setButtonItemsF(buttons, 1);

}

void VRAMTestSuiteConsole::_executeOnce() {
  cls();
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("=== VRAM TEST SUITE ==="));
  println();
  setTextColor(0xFFFF, 0x0000);
  println(F("Testing Video RAM (1KB)"));
  println(F("Memory Range: 0x3C00-0x3FFF"));
  println(F("IC References: Z48,Z47,Z46,Z45,Z61,Z62,Z?,Z63"));
  println();

  setTextColor(0xF800, 0x0000);  // Red
  println(F("WARNING: Screen may flicker during"));
  println(F("video memory testing!"));
  println();

  setTextColor(0xF81F, 0x0000);  // Magenta
  println(F("Starting VRAM comprehensive test..."));
  println();

  delay(5000);

  cls();
  setTextColor(0xFFFF, 0x0000);  // White

  // Local VRAM constants - only exist when test is running
  const uint16_t start = 0x3C00;  // VRAM start address
  const uint16_t length = 1024;   // 1KB VRAM
  const char *const icRefs[] PROGMEM = {"Z48", "Z47", "Z46", "Z45", "Z61", "Z62", "Z?", "Z63"};

  // Run the comprehensive test suite on VRAM
  runSpecializedTest(start, length, icRefs);
}

Screen *VRAMTestSuiteConsole::actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) {
  if (action & BUTTON_MENU) {
    return new VideoMenu();
  }

  return nullptr;
}
