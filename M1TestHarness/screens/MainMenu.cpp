#include "./MainMenu.h"

#include <Arduino.h>

#include "../globals.h"
#include "./BoardRevisionMenu.h"
#include "./about/AboutConsole.h"
#include "./advanced/AdvancedMenu.h"
#include "./cassette/CassetteMenu.h"
#include "./dram/DRAMMenu.h"
#include "./keyboard/KeyboardTester.h"
#include "./rom/ROMMenu.h"
#include "./video/VideoMenu.h"

MainMenu::MainMenu() {
  setTitle((const char*)F("Main Menu"));
  const char* menuItems[] = {(const char*)F("Board Revision"), (const char*)F("One-Step Tests"),
                             (const char*)F("DRAM"),           (const char*)F("ROM"),
                             (const char*)F("Cassette"),       (const char*)F("Video"),
                             (const char*)F("Keyboard"),       (const char*)F("Advanced"),
                             (const char*)F("About")};
  setMenuItems(menuItems, 9);

  Globals.logger.infoF(F("Main Menu initialized"));
}

Screen* MainMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Board Revision
      Globals.logger.infoF(F("Opening Board Revision Menu"));
      return new BoardRevisionMenu(BoardRevisionSource::MAIN_MENU);

    case 1:  // One-Step Tests
      Globals.logger.infoF(F("Opening One-Step Tests Console"));
      return nullptr;
      // return new OneStepTestsScreen();

    case 2:  // DRAM
      Globals.logger.infoF(F("Opening DRAM Menu"));
      return new DRAMMenu();

    case 3:  // ROM
      Globals.logger.infoF(F("Opening ROM Menu"));
      return new ROMMenu();

    case 4:  // Cassette
      Globals.logger.infoF(F("Opening Cassette Menu"));
      return new CassetteMenu();

    case 5:  // Video
      Globals.logger.infoF(F("Opening Video Menu"));
      return new VideoMenu();

    case 6:  // Keyboard
      Globals.logger.infoF(F("Opening Keyboard Menu"));
      return new KeyboardTester();

    case 7:  // Advanced
      Globals.logger.infoF(F("Opening Advanced Menu"));
      return new AdvancedMenu();

    case 8:  // About
      Globals.logger.infoF(F("Opening About Screen"));
      return new AboutConsole();

    default:
      // No action for invalid index
      return nullptr;
  }
}

const char* MainMenu::_getMenuItemConfigValue(uint8_t index) {
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
