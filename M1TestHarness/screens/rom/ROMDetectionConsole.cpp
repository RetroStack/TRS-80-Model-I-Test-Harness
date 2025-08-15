#include "./ROMDetectionConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./ROMMenu.h"

ROMDetectionConsole::ROMDetectionConsole() : ConsoleScreen() {
  setTitle((const char *)F("ROM Detection"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  // Set button labels
  const char *buttons[] = {(const char *)F("M:Back")};
  setButtonItems(buttons, 1);

  Globals.logger.info(F("ROM Detection Demo initialized"));
}

void ROMDetectionConsole::_executeOnce() {
  cls();
  setTextColor(0x07E0, 0x0000);  // Green
  println(F("=== ROM DETECTION CONSOLE ==="));
  println();
  setTextColor(0xFFFF, 0x0000);
  println(F("Analyzing ROM contents..."));
  println();
  Model1.activateTestSignal();
  uint16_t a = Globals.rom.getChecksum(0);
  uint16_t b = Globals.rom.getChecksum(1);
  uint16_t c = Globals.rom.getChecksum(2);
  uint16_t d = Globals.rom.getChecksum(3);
  Model1.deactivateTestSignal();

  setTextColor(0xFFFF, 0x0000);
  print(F("Checksum ROM A: "));
  setTextColor(0x07FF, 0x0000);
  println(a, HEX);

  setTextColor(0xFFFF, 0x0000);
  print(F("Checksum ROM B: "));
  setTextColor(0x07FF, 0x0000);
  println(b, HEX);

  setTextColor(0xFFFF, 0x0000);
  print(F("Checksum ROM C: "));
  setTextColor(0x07FF, 0x0000);
  println(c, HEX);

  setTextColor(0xFFFF, 0x0000);
  print(F("Checksum ROM D: "));
  setTextColor(0x07FF, 0x0000);
  println(d, HEX);

  // Try to detect ROM type
  Model1.activateTestSignal();
  const __FlashStringHelper *romType = Globals.rom.identifyROM();
  Model1.deactivateTestSignal();
  print(F("ROM Type: "));
  if (romType != nullptr) {
    setTextColor(0x07E0, 0x0000);  // Green
    println(romType);
  } else {
    setTextColor(0xF800, 0x0000);  // Red
    println(F("Unknown"));
  }

  println();
  setTextColor(0xF81F, 0x0000);  // Magenta
  println(F("Detection complete!"));
}

Screen *ROMDetectionConsole::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_MENU) {
    Globals.logger.info(F("Returning to ROM Menu"));
    return new ROMMenu();
  }

  return nullptr;
}
