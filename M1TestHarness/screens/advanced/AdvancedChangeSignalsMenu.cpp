#include "./AdvancedChangeSignalsMenu.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "./AdvancedMenu.h"
#include "./AdvancedSignalController.h"

AdvancedChangeSignalsMenu::AdvancedChangeSignalsMenu() : MenuScreen() {
  setTitleF(F("Change Signals"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const __FlashStringHelper *menuItems[] = {
      F("Test Signal"), F("Address"),         F("Address Count"), F("Data"),
      F("Data Count"),  F("RAS Signal"),      F("CAS Signal"),    F("MUX Signal"),
      F("Read Signal"), F("Write Signal"),    F("In Signal"),     F("Out Signal"),
      F("Wait Signal"), F("Interrupt Signal")};
  setMenuItemsF(menuItems, 14);

  Globals.logger.infoF(F("Change Signals Menu initialized"));
}

bool AdvancedChangeSignalsMenu::open() {
  // Start the Advanced Signal Controller
  AdvancedSignals.begin();

  // Call parent implementation
  return MenuScreen::open();
}

void AdvancedChangeSignalsMenu::close() {
  // Call parent implementation
  MenuScreen::close();

  // Note: We don't stop the AdvancedSignalController here because we want
  // the signal configuration to persist across screens in the Advanced menu
}

void AdvancedChangeSignalsMenu::loop() {
  // Call parent loop first
  MenuScreen::loop();

  // Let the global signal controller handle timing and signal updates
  AdvancedSignals.loop();
}

Screen *AdvancedChangeSignalsMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Test Signal
      _toggleTestSignal();
      return nullptr;
    case 1:  // Address
      _toggleAddressMode();
      return nullptr;
    case 2:  // Address Count
      _toggleAddressCountDuration();
      return nullptr;
    case 3:  // Data
      _toggleDataMode();
      return nullptr;
    case 4:  // Data Count
      _toggleDataCountDuration();
      return nullptr;
    case 5:  // RAS Signal
      _toggleRasSignal();
      return nullptr;
    case 6:  // CAS Signal
      _toggleCasSignal();
      return nullptr;
    case 7:  // MUX Signal
      _toggleMuxSignal();
      return nullptr;
    case 8:  // Read Signal
      _toggleReadSignal();
      return nullptr;
    case 9:  // Write Signal
      _toggleWriteSignal();
      return nullptr;
    case 10:  // In Signal
      _toggleInSignal();
      return nullptr;
    case 11:  // Out Signal
      _toggleOutSignal();
      return nullptr;
    case 12:  // Wait Signal
      _toggleWaitSignal();
      return nullptr;
    case 13:  // Interrupt Signal
      _toggleInterruptSignal();
      return nullptr;

    case -1:  // Back to menu
      Globals.logger.infoF(F("Returning to Advanced Menu from Advanced Signal Change Menu"));
      return new AdvancedMenu();

    default:
      return nullptr;
  }
}

const __FlashStringHelper *AdvancedChangeSignalsMenu::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 0:  // Test Signal
      return AdvancedSignals.isTestSignalActive() ? F("On") : F("Off");
    case 1:  // Address
      return AdvancedSignals.getAddressModeString();
    case 2:  // Address Count
      return AdvancedSignals.getAddressCountDurationString();
    case 3:  // Data
      return AdvancedSignals.getDataModeString();
    case 4:  // Data Count
      return AdvancedSignals.getDataCountDurationString();
    case 5:  // RAS Signal
      return AdvancedSignals.getRasSignal() ? F("On") : F("Off");
    case 6:  // CAS Signal
      return AdvancedSignals.getCasSignal() ? F("On") : F("Off");
    case 7:  // MUX Signal
      return AdvancedSignals.getMuxSignal() ? F("On") : F("Off");
    case 8:  // Read Signal
      return AdvancedSignals.getReadSignal() ? F("On") : F("Off");
    case 9:  // Write Signal
      return AdvancedSignals.getWriteSignal() ? F("On") : F("Off");
    case 10:  // In Signal
      return AdvancedSignals.getInSignal() ? F("On") : F("Off");
    case 11:  // Out Signal
      return AdvancedSignals.getOutSignal() ? F("On") : F("Off");
    case 12:  // Wait Signal
      return AdvancedSignals.getWaitSignal() ? F("On") : F("Off");
    case 13:  // Interrupt Signal
      return AdvancedSignals.getInterruptSignal() ? F("On") : F("Off");
    default:
      return nullptr;
  }
}

bool AdvancedChangeSignalsMenu::_isMenuItemEnabled(uint8_t index) const {
  switch (index) {
    case 0:   // Test Signal - always enabled
    case 12:  // Wait Signal - always enabled
    case 13:  // Interrupt Signal - always enabled
      return true;
    case 2:
      if (!AdvancedSignals.isTestSignalActive())
        return false;
      return AdvancedSignals.getAddressMode() == 4;
    case 4:
      if (!AdvancedSignals.isTestSignalActive())
        return false;
      return AdvancedSignals.getDataMode() == 4;
    default:
      // All other items require test signal to be active
      return AdvancedSignals.isTestSignalActive();
  }
}

// Toggle methods implementation
void AdvancedChangeSignalsMenu::_toggleAddressMode() {
  AdvancedSignals.toggleAddressMode();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleAddressCountDuration() {
  AdvancedSignals.toggleAddressCountDuration();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleDataMode() {
  AdvancedSignals.toggleDataMode();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleDataCountDuration() {
  AdvancedSignals.toggleDataCountDuration();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleRasSignal() {
  AdvancedSignals.toggleRasSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleCasSignal() {
  AdvancedSignals.toggleCasSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleMuxSignal() {
  AdvancedSignals.toggleMuxSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleReadSignal() {
  AdvancedSignals.toggleReadSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleWriteSignal() {
  AdvancedSignals.toggleWriteSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleInSignal() {
  AdvancedSignals.toggleInSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleOutSignal() {
  AdvancedSignals.toggleOutSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleWaitSignal() {
  AdvancedSignals.toggleWaitSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleInterruptSignal() {
  AdvancedSignals.toggleInterruptSignal();
  _drawContent();
}

void AdvancedChangeSignalsMenu::_toggleTestSignal() {
  AdvancedSignals.toggleTestSignal();
  _drawContent();
}
