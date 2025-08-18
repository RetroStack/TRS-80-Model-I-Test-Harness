#include "./AdvancedMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./AdvancedSignalController.h"
#include "./SignalGenerator.h"
#include "./SignalOscilloscope.h"

AdvancedMenu::AdvancedMenu() : MenuScreen() {
  setTitleF(F("Advanced"));

  const __FlashStringHelper *menuItems[] = {F("Oscilloscope"), F("Signal Generator")};
  setMenuItemsF(menuItems, 2);

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
      return new SignalGenerator();

    case -1:  // Back to Main
      AdvancedSignals.end();  // Stop signal controller when leaving Advanced system
      return new MainMenu();

    default:
      return nullptr;
  }
}
