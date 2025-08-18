#include "./MainMenu.h"

#include <Arduino.h>

#include "../globals.h"
#include "./BoardRevisionMenu.h"
#include "./HardwareDetectionConsole.h"
#include "./about/AboutConsole.h"
#include "./advanced/AdvancedMenu.h"
#include "./advanced/AdvancedSignalController.h"
#include "./cassette/CassetteMenu.h"
#include "./dram/DRAMMenu.h"
#include "./keyboard/KeyboardTester.h"
#include "./rom/ROMMenu.h"
#include "./video/VideoMenu.h"

MainMenu::MainMenu() {
  setTitleF(F("Main Menu"));
  const __FlashStringHelper* menuItems[] = {
      F("Board Revision"), F("Hardware Detection"), F("DRAM"),     F("ROM"),  F("Cassette"),
      F("Video"),          F("Keyboard"),           F("Advanced"), F("About")};
  setMenuItemsF(menuItems, 9);
}

Screen* MainMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Board Revision
      return new BoardRevisionMenu(BoardRevisionSource::MAIN_MENU);

    case 1:  // Hardware Detection
      return new HardwareDetectionConsole();

    case 2:  // DRAM
      return new DRAMMenu();

    case 3:  // ROM
      return new ROMMenu();

    case 4:  // Cassette
      return new CassetteMenu();

    case 5:  // Video
      return new VideoMenu();

    case 6:  // Keyboard
      return new KeyboardTester();

    case 7:  // Advanced
      AdvancedSignals.begin();
      return new AdvancedMenu();

    case 8:  // About
      return new AboutConsole();

    default:
      // No action for invalid index
      return nullptr;
  }
}

const __FlashStringHelper* MainMenu::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 0:  // Board Revision
      return Globals.getBoardRevisionString(Globals.getBoardRevision());
    default:
      return nullptr;
  }
}

bool MainMenu::_isMenuItemEnabled(uint8_t index) const {
  switch (index) {
    case 1:          // One-Step Tests
      return false;  // One-Step Tests is not enabled for now
    default:
      return true;
  }
}
