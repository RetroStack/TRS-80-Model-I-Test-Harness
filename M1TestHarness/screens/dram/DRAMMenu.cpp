#include "./DRAMMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./DRAMContentViewerConsole.h"
#include "./DRAMTestSuiteConsole.h"

// Static buffer for dynamic string formatting
char DRAMMenu::_configBuffer[16];

DRAMMenu::DRAMMenu() : MenuScreen() {
  setTitleF(F("DRAM"));

  // Create menu items for DRAM features - copy from PROGMEM
  const __FlashStringHelper *menuItems[] = {F("Memory Size"), F("DRAM Viewer"),
                                            F("DRAM Test Suite")};
  setMenuItemsF(menuItems, 3);

  // Initialize DRAM size values - will be set properly in open()
  _currentDRAMSizeKB = 0;

  Globals.logger.infoF(F("DRAM Menu screen initialized"));
}

bool DRAMMenu::open() {
  // Read current DRAM size from globals
  _currentDRAMSizeKB = Globals.getDRAMSizeKB();

  Globals.logger.infoF(F("DRAM Menu opened - Current: %d KB, Detected: %d KB"), _currentDRAMSizeKB,
                       Globals.getDetectedDRAMSizeKB());

  // Call parent implementation
  return MenuScreen::open();
}

Screen *DRAMMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Memory Size - Toggle between standard sizes
      toggleDRAMSize();
      return nullptr;  // Stay on this screen

    case 1:  // DRAM Viewer
      Globals.logger.infoF(F("Opening DRAM Content Viewer"));
      return new DRAMContentViewerConsole();

    case 2:  // DRAM Test Suite
      Globals.logger.infoF(F("Opening DRAM Test Suite"));
      return new DRAMTestSuiteConsole();

    case -1:  // Back
      return new MainMenu();

    default:
      return nullptr;
  }
}

const char *DRAMMenu::_getMenuItemConfigValue(uint8_t index) {
  switch (index) {
    case 0:  // Memory Size
      // Check if this is the originally detected size and it's not a standard size
      const uint16_t standardSizes[] = {4, 8, 16, 32, 48};
      const uint8_t numStandardSizes = 5;

      bool isStandardSize = false;
      for (uint8_t i = 0; i < numStandardSizes; i++) {
        if (_currentDRAMSizeKB == standardSizes[i]) {
          isStandardSize = true;
          break;
        }
      }

      // Get the originally detected size from Globals
      uint16_t detectedSize = Globals.getDetectedDRAMSizeKB();

      // If this is a non-standard size and matches the originally detected size,
      // show it with "*" suffix
      if (!isStandardSize && _currentDRAMSizeKB == detectedSize) {
        snprintf(_configBuffer, sizeof(_configBuffer), "%dKB*", _currentDRAMSizeKB);
      } else {
        snprintf(_configBuffer, sizeof(_configBuffer), "%dKB", _currentDRAMSizeKB);
      }

      return _configBuffer;
    default:
      return nullptr;
  }
}

void DRAMMenu::toggleDRAMSize() {
  // Standard TRS-80 DRAM configurations
  const uint16_t standardSizes[] = {4, 8, 16, 32, 48};
  const uint8_t numStandardSizes = 5;

  // Get the originally detected size
  uint16_t detectedSize = Globals.getDetectedDRAMSizeKB();

  // Check if current size is one of the standard sizes
  int currentIndex = -1;
  for (uint8_t i = 0; i < numStandardSizes; i++) {
    if (_currentDRAMSizeKB == standardSizes[i]) {
      currentIndex = i;
      break;
    }
  }

  if (currentIndex >= 0) {
    // Current size is standard, move to next standard size
    currentIndex = (currentIndex + 1) % numStandardSizes;
    _currentDRAMSizeKB = standardSizes[currentIndex];

    // After cycling through all standard sizes, offer the detected size if it's different
    if (currentIndex == 0) {  // We've wrapped around to 4KB
      // Check if detected size is different from any standard size
      bool detectedIsStandard = false;
      for (uint8_t i = 0; i < numStandardSizes; i++) {
        if (detectedSize == standardSizes[i]) {
          detectedIsStandard = true;
          break;
        }
      }

      // If detected size is non-standard, use it instead of 4KB
      if (!detectedIsStandard && detectedSize > 0) {
        _currentDRAMSizeKB = detectedSize;
      }
    }
  } else {
    // Current size is not standard (likely the detected odd size), move to first standard size
    _currentDRAMSizeKB = standardSizes[0];  // Start with 4KB
  }

  // Update globals with new size
  Globals.setDRAMSizeKB(_currentDRAMSizeKB);

  // Log the change
  Globals.logger.infoF(F("DRAM size toggled to: %d KB"), _currentDRAMSizeKB);

  // Redraw the menu to show updated config value
  _drawContent();
}
