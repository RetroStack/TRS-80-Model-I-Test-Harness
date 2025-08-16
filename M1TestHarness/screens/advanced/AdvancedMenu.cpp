#include "./AdvancedMenu.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./AdvancedChangeSignalsMenu.h"
#include "./SignalOscilloscope.h"

AdvancedMenu::AdvancedMenu() : MenuScreen() {
  setTitleF(F("Advanced"));

  const __FlashStringHelper *menuItems[] = {F("Signal Oscilloscope"), F("Change Signals")};
  setMenuItemsF(menuItems, 2);

  Globals.logger.infoF(F("Advanced Menu initialized"));
}

Screen *AdvancedMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Signal Oscilloscope
      return new SignalOscilloscope();
    case 1:  // Change Signals
      return new AdvancedChangeSignalsMenu();

    case -1:  // Back to Main
      Globals.logger.infoF(F("Returning to main menu from Advanced Menu"));
      return new MainMenu();

    default:
      return nullptr;
  }
}
