#include "./DRAMMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./DRAMTestSuiteConsole.h"

DRAMMenu::DRAMMenu() : MenuScreen() {
  setTitleF(F("DRAM"));

  // Create menu items for DRAM features - copy from PROGMEM
  const __FlashStringHelper *menuItems[] = {F("Memory Size"), F("DRAM Test Suite")};
  setMenuItemsF(menuItems, 2);

  Globals.logger.infoF(F("DRAM Menu screen initialized"));
}

Screen *DRAMMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Memory Size - Disabled config menu
      Globals.logger.infoF(F("Memory Size - Configuration disabled"));
      return nullptr;

    case 1:  // DRAM Test Suite
      Globals.logger.infoF(F("Opening DRAM Test Suite"));
      return new DRAMTestSuiteConsole();

    case -1:  // Back
      return new MainMenu();

    default:
      return nullptr;
  }
}

const __FlashStringHelper *DRAMMenu::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 0:             // Memory Size
      return F("16K");  // TODO: Get actual memory size detection
    default:
      return nullptr;
  }
}
