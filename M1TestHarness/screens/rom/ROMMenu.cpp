#include "./ROMMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./ROMContentViewerConsole.h"
#include "./ROMDetectionConsole.h"

ROMMenu::ROMMenu() : MenuScreen() {
  setTitle((const char *)F("ROM"));

  // Create menu items for ROM features - copy from PROGMEM
  const char *menuItems[] = {(const char *)F("ROM Detection"), (const char *)F("ROM Contents")};
  setMenuItems(menuItems, 2);

  Globals.logger.info(F("ROM Menu screen initialized"));
}

Screen *ROMMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // ROM Detection
      Globals.logger.info(F("Opening ROM Detection"));
      return new ROMDetectionConsole();

    case 1:  // ROM Contents
      Globals.logger.info(F("Opening ROM Contents"));
      return new ROMContentViewerConsole();

    case -1:  // Back
      return new MainMenu();

    default:
      return nullptr;
  }
}
