#include "./SignalGenerator.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "./AdvancedMenu.h"
#include "./AdvancedSignalController.h"

SignalGenerator::SignalGenerator() : MenuScreen() {
  setTitleF(F("Signal Generator"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const __FlashStringHelper *menuItems[] = {
      F("Test Signal"), F("Address"),         F("Address Count"), F("Data"),
      F("Data Count"),  F("RAS Signal"),      F("CAS Signal"),    F("MUX Signal"),
      F("Read Signal"), F("Write Signal"),    F("In Signal"),     F("Out Signal"),
      F("Wait Signal"), F("Interrupt Signal")};
  setMenuItemsF(menuItems, 14);

  Globals.logger.infoF(F("Signal Generator Menu initialized"));
}

bool SignalGenerator::open() {
  // Start the Advanced Signal Controller
  AdvancedSignals.begin();

  // Call parent implementation
  return MenuScreen::open();
}

void SignalGenerator::close() {
  // Call parent implementation
  MenuScreen::close();

  // Note: We don't stop the AdvancedSignalController here because we want
  // the signal configuration to persist across screens in the Advanced menu
}

void SignalGenerator::loop() {
  // Call parent loop first
  MenuScreen::loop();

  // Let the global signal controller handle timing and signal updates
  AdvancedSignals.loop();
}

Screen *SignalGenerator::_getSelectedMenuItemScreen(int index) {
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

const __FlashStringHelper *SignalGenerator::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 0:  // Test Signal
      return AdvancedSignals.isTestSignalActive() ? F("Active") : F("Inactive");
    case 1:  // Address
      return AdvancedSignals.getAddressModeString();
    case 2:  // Address Count
      return AdvancedSignals.getAddressCountDurationString();
    case 3:  // Data
      return AdvancedSignals.getDataModeString();
    case 4:  // Data Count
      return AdvancedSignals.getDataCountDurationString();
    case 5:  // RAS Signal
      return AdvancedSignals.getRasSignalModeString();
    case 6:  // CAS Signal
      return AdvancedSignals.getCasSignalModeString();
    case 7:  // MUX Signal
      return AdvancedSignals.getMuxSignalModeString();
    case 8:  // Read Signal
      return AdvancedSignals.getReadSignalModeString();
    case 9:  // Write Signal
      return AdvancedSignals.getWriteSignalModeString();
    case 10:  // In Signal
      return AdvancedSignals.getInSignalModeString();
    case 11:  // Out Signal
      return AdvancedSignals.getOutSignalModeString();
    case 12:  // Wait Signal
      return AdvancedSignals.getWaitSignalModeString();
    case 13:  // Interrupt Signal
      return AdvancedSignals.getInterruptSignalModeString();
    default:
      return nullptr;
  }
}

bool SignalGenerator::_isMenuItemEnabled(uint8_t index) const {
  switch (index) {
    case 0:   // Test Signal - always enabled
    case 12:  // Wait Signal - always enabled
    case 13:  // Interrupt Signal - always enabled
      return true;
    case 2:
      if (!AdvancedSignals.isTestSignalActive())
        return false;
      return AdvancedSignals.getAddressMode() == 5;  // Count mode is now 5
    case 4:
      if (!AdvancedSignals.isTestSignalActive())
        return false;
      return AdvancedSignals.getDataMode() == 5;  // Count mode is now 5
    default:
      // All other items require test signal to be active
      return AdvancedSignals.isTestSignalActive();
  }
}

// Toggle methods implementation
void SignalGenerator::_toggleAddressMode() {
  AdvancedSignals.toggleAddressMode();
  _drawContent();
}

void SignalGenerator::_toggleAddressCountDuration() {
  AdvancedSignals.toggleAddressCountDuration();
  _drawContent();
}

void SignalGenerator::_toggleDataMode() {
  AdvancedSignals.toggleDataMode();
  _drawContent();
}

void SignalGenerator::_toggleDataCountDuration() {
  AdvancedSignals.toggleDataCountDuration();
  _drawContent();
}

void SignalGenerator::_toggleRasSignal() {
  AdvancedSignals.toggleRasSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleCasSignal() {
  AdvancedSignals.toggleCasSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleMuxSignal() {
  AdvancedSignals.toggleMuxSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleReadSignal() {
  AdvancedSignals.toggleReadSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleWriteSignal() {
  AdvancedSignals.toggleWriteSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleInSignal() {
  AdvancedSignals.toggleInSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleOutSignal() {
  AdvancedSignals.toggleOutSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleWaitSignal() {
  AdvancedSignals.toggleWaitSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleInterruptSignal() {
  AdvancedSignals.toggleInterruptSignalMode();
  _drawContent();
}

void SignalGenerator::_toggleTestSignal() {
  AdvancedSignals.toggleTestSignal();
  _drawContent();
}
