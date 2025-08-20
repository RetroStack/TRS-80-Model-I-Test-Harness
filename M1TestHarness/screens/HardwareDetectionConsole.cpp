#include "./HardwareDetectionConsole.h"

#include <Arduino.h>
#include <M1Shield.h>
#include <Model1.h>

#include "../globals.h"
#include "./MainMenu.h"

HardwareDetectionConsole::HardwareDetectionConsole() : ConsoleScreen() {
  setTitleF(F("Auto Detection"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  clearButtonItems();

  // Enable auto-forward after 5 seconds
  setAutoForward(true, 5000);
}

void HardwareDetectionConsole::_executeOnce() {
  setTextColor(0x07FF, 0x0000);  // Cyan
  println(F("Detecting hardware configuration..."));
  println();

  // Set LED to blue to indicate detection in progress
  M1Shield.setLEDColor(COLOR_BLUE);

  // Perform detections
  detectBoardRevision();
  detectLowerCaseMod();
  detectDRAMSize();

  // Display results
  displayResults();

  // Set LED to green when complete
  M1Shield.setLEDColor(COLOR_GREEN);

  setTextColor(0xFFFF, 0x0000);  // White
  println();
  println(F("Detection complete! Press MENU to"));
  println(F("continue to main menu."));
  println();

  // Use notification instead of manual println for auto-forward message
  notifyF(F("Auto-forward in 5 seconds"), 5000);
}

Screen *HardwareDetectionConsole::actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) {
  if (action & (BUTTON_ANY | BUTTON_MENU)) {
    // Turn off LED when exiting
    M1Shield.setLEDColor(COLOR_OFF);
    return new MainMenu();
  }

  return nullptr;
}

void HardwareDetectionConsole::detectBoardRevision() {
  setTextColor(0xF81F, 0x0000);  // Magenta
  print(F("Board Revision: "));

  // For now, set to UNKNOWN as requested
  // TODO: Implement actual board revision detection logic
  Globals.setBoardRevision(UNKNOWN);

  setTextColor(0xFFE0, 0x0000);  // Yellow
  println(Globals.getBoardRevisionString(Globals.getBoardRevision()));

  Globals.logger.infoF(F("Board revision set to UNKNOWN (default)"));
}

void HardwareDetectionConsole::detectLowerCaseMod() {
  setTextColor(0xF81F, 0x0000);  // Magenta
  print(F("Lower-case Mod: "));

  // Test for lowercase modification by writing 0x61 ('a') to first video position
  Model1.activateTestSignal();

  // Save original value at video position 0x3C00
  uint8_t originalValue = Model1.readMemory(0x3C00);

  // Write lowercase 'a' (0x61) to first video position
  Model1.writeMemory(0x3C00, 0x61);
  delay(10);  // Allow time for write to settle

  // Read back the value
  uint8_t readBackValue = Model1.readMemory(0x3C00);

  // Restore original value
  Model1.writeMemory(0x3C00, originalValue);

  Model1.deactivateTestSignal();

  // Check if the value matches what we wrote
  bool hasLowerCase = (readBackValue == 0x61);
  Globals.setHasLowerCaseMod(hasLowerCase);

  setTextColor(0xFFE0, 0x0000);  // Yellow
  if (hasLowerCase) {
    println(F("Yes"));
    Globals.logger.infoF(F("Lower-case modification detected"));
  } else {
    println(F("No"));
    Globals.logger.infoF(F("Lower-case modification not detected"));
  }
}

void HardwareDetectionConsole::detectDRAMSize() {
  setTextColor(0xF81F, 0x0000);  // Magenta
  print(F("DRAM Size: "));

  Model1.activateTestSignal();

  uint16_t detectedSize = 0;

  // Test DRAM in 4KB increments from 0x4000 upward
  // Standard TRS-80 configurations: 4KB, 16KB, 32KB, 48KB
  for (uint16_t testSize = 4; testSize <= 48; testSize += 4) {
    uint16_t testAddress = 0x4000 + ((testSize - 4) * 1024);

    // Don't test beyond reasonable DRAM range
    if (testAddress >= 0xC000)
      break;

    // Save original values
    uint8_t originalValue1 = Model1.readMemory(testAddress);
    uint8_t originalValue2 = Model1.readMemory(testAddress + 1);

    // Write test patterns
    Model1.writeMemory(testAddress, 0xAA);
    Model1.writeMemory(testAddress + 1, 0x55);
    delay(5);

    // Read back and verify
    uint8_t readBack1 = Model1.readMemory(testAddress);
    uint8_t readBack2 = Model1.readMemory(testAddress + 1);

    // Restore original values
    Model1.writeMemory(testAddress, originalValue1);
    Model1.writeMemory(testAddress + 1, originalValue2);

    // Check if memory responded correctly
    if (readBack1 == 0xAA && readBack2 == 0x55) {
      detectedSize = testSize;
    } else {
      break;  // Memory test failed, we've found the limit
    }
  }

  Model1.deactivateTestSignal();

  // Default to 16KB if no DRAM detected (standard configuration)
  if (detectedSize == 0) {
    detectedSize = 16;
  }

  Globals.setDetectedDRAMSizeKB(detectedSize);  // Also store as detected size

  setTextColor(0xFFE0, 0x0000);  // Yellow
  print(detectedSize);
  println(F("KB"));

  Globals.logger.infoF(F("DRAM size detected: %d KB"), detectedSize);
}

void HardwareDetectionConsole::displayResults() {
  println();
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("=== DETECTION RESULTS ==="));

  setTextColor(0xFFFF, 0x0000);  // White
  print(F("Board Revision: "));
  setTextColor(0xFFE0, 0x0000);  // Yellow
  println(Globals.getBoardRevisionString(Globals.getBoardRevision()));

  setTextColor(0xFFFF, 0x0000);  // White
  print(F("Lower-case Mod: "));
  setTextColor(0xFFE0, 0x0000);  // Yellow
  println(Globals.getHasLowerCaseMod() ? F("Yes") : F("No"));

  setTextColor(0xFFFF, 0x0000);  // White
  print(F("DRAM Size: "));
  setTextColor(0xFFE0, 0x0000);  // Yellow
  print(Globals.getDRAMSizeKB());
  println(F("KB"));
}
