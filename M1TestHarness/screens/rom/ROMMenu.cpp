#include "./ROMMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./ROMContentViewerConsole.h"
#include "./ROMDetectionConsole.h"

ROMMenu::ROMMenu() : MenuScreen() {
  setTitleF(F("ROM"));

  // Create menu items for ROM features - copy from PROGMEM
  const __FlashStringHelper *menuItems[] = {F("ROM Detection"), F("ROM Contents")};
  setMenuItemsF(menuItems, 2);

}

Screen *ROMMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // ROM Detection
      return new ROMDetectionConsole();

    case 1:  // ROM Contents
      return new ROMContentViewerConsole();

    case -1:  // Back
      return new MainMenu();

    default:
      return nullptr;
  }
}
