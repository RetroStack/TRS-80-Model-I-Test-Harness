#include "DRAMMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "DRAMTestSuiteConsole.h"

DRAMMenu::DRAMMenu() : MenuScreen() {
  setTitle((const char *)F("DRAM"));

  // Create menu items for DRAM features - copy from PROGMEM
  const char *menuItems[] = {(const char *)F("Memory Size"), (const char *)F("DRAM Test Suite")};
  setMenuItems(menuItems, 2);

  Globals.logger.info(F("DRAM Menu screen initialized"));
}

Screen *DRAMMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Memory Size - Disabled config menu
      Globals.logger.info(F("Memory Size - Configuration disabled"));
      return nullptr;

    case 1:  // DRAM Test Suite
      Globals.logger.info(F("Opening DRAM Test Suite"));
      return new DRAMTestSuiteConsole();

    case -1:  // Back
      return new MainMenu();

    default:
      return nullptr;
  }
}

const char *DRAMMenu::_getMenuItemConfigValue(uint8_t index) {
  switch (index) {
    case 0:          // Memory Size
      return "16K";  // TODO: Get actual memory size detection
    default:
      return nullptr;
  }
}
