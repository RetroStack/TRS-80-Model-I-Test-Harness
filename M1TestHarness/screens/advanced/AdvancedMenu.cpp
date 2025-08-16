#include "./AdvancedMenu.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./AdvancedChangeSignalsMenu.h"
#include "./AdvancedSignalController.h"
#include "./SignalOscilloscope.h"

AdvancedMenu::AdvancedMenu() : MenuScreen() {
  setTitleF(F("Advanced"));

  const __FlashStringHelper *menuItems[] = {F("Signal Oscilloscope"), F("Change Signals")};
  setMenuItemsF(menuItems, 2);

  Globals.logger.infoF(F("Advanced Menu initialized"));
}

bool AdvancedMenu::open() {
  // Start the Advanced Signal Controller when entering Advanced menu system
  AdvancedSignals.begin();

  // Call parent implementation
  return MenuScreen::open();
}

void AdvancedMenu::close() {
  // Call parent implementation
  MenuScreen::close();

  // Stop the Advanced Signal Controller when leaving Advanced menu system
  AdvancedSignals.end();
}

void AdvancedMenu::loop() {
  // Call parent loop first
  MenuScreen::loop();

  // Let the global signal controller handle signal updates
  AdvancedSignals.loop();
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
