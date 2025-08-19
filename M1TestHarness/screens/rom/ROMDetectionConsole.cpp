#include "./ROMDetectionConsole.h"

#include <Arduino.h>
#include <M1Shield.h>

#include "../../globals.h"
#include "./ROMMenu.h"

ROMDetectionConsole::ROMDetectionConsole() : ConsoleScreen() {
  setTitleF(F("ROM Detection"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  // Set button labels
  const __FlashStringHelper *buttons[] = {F("M:Back")};
  setButtonItemsF(buttons, 1);

}

void ROMDetectionConsole::_executeOnce() {
  cls();
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("=== ROM DETECTION CONSOLE ==="));
  println();
  setTextColor(0xFFFF, 0x0000);
  println(F("Analyzing ROM contents..."));
  println();

  // Initialize progress and LED
  setProgressValue(0);
  M1Shield.setLEDColor(COLOR_BLUE);  // Starting ROM detection

  Model1.activateTestSignal();

  // Step 1: Read ROM A checksum
  setProgressValue(10);
  M1Shield.setLEDColor(COLOR_CYAN);  // Reading ROM A
  setTextColor(0x07FF, 0x0000);      // Cyan
  print(F("Reading ROM A..."));
  uint16_t a = Globals.rom.getChecksum(0);
  println(F(" done"));

  // Step 2: Read ROM B checksum
  setProgressValue(30);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Reading ROM B
  setTextColor(0x07FF, 0x0000);         // Cyan
  print(F("Reading ROM B..."));
  uint16_t b = Globals.rom.getChecksum(1);
  println(F(" done"));

  // Step 3: Read ROM C checksum
  setProgressValue(50);
  M1Shield.setLEDColor(COLOR_CYAN);  // Reading ROM C
  setTextColor(0x07FF, 0x0000);      // Cyan
  print(F("Reading ROM C..."));
  uint16_t c = Globals.rom.getChecksum(2);
  println(F(" done"));

  // Step 4: Read ROM D checksum
  setProgressValue(70);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Reading ROM D
  setTextColor(0x07FF, 0x0000);         // Cyan
  print(F("Reading ROM D..."));
  uint16_t d = Globals.rom.getChecksum(3);
  println(F(" done"));

  println();

  // Display checksums
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

  // Step 5: ROM identification
  setProgressValue(85);
  M1Shield.setLEDColor(COLOR_YELLOW);  // Analyzing ROM type
  setTextColor(0x07FF, 0x0000);        // Cyan
  print(F("Identifying ROM type..."));
  const __FlashStringHelper *romType = Globals.rom.identifyROM();
  println(F(" done"));

  Model1.deactivateTestSignal();

  print(F("ROM Type: "));
  if (romType != nullptr) {
    setTextColor(0x07E0, 0x0000);  // Green
    println(romType);
    setProgressValue(100);
    M1Shield.setLEDColor(COLOR_GREEN);  // Success - ROM identified
  } else {
    setTextColor(0xF800, 0x0000);  // Red
    println(F("Unknown"));
    setProgressValue(100);
    M1Shield.setLEDColor(COLOR_RED);  // Failure - unknown ROM
  }

  println();
  setTextColor(0xF81F, 0x0000);  // Magenta
  println(F("Detection complete!"));

  // Hold final status for 2 seconds, then turn off LED
  delay(2000);
  M1Shield.setLEDColor(COLOR_OFF);
}

Screen *ROMDetectionConsole::actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) {
  if (action & BUTTON_MENU) {
    return new ROMMenu();
  }

  return nullptr;
}
