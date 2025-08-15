#include "AdvancedMenu.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "../MainMenu.h"

AdvancedMenu::AdvancedMenu() : MenuScreen() {
  setTitle((const char *)F("Advanced"));

  // Initialize state variables
  _addressMode = 0;           // Default to 0x00
  _addressCountDuration = 0;  // Default to 1s
  _dataMode = 0;              // Default to 0x00
  _dataCountDuration = 0;     // Default to 1s
  _rasSignal = false;
  _casSignal = false;
  _muxSignal = false;
  _readSignal = false;
  _writeSignal = false;
  _inSignal = false;
  _outSignal = false;
  _waitSignal = false;
  _interruptSignal = false;
  _testSignalActive = false;

  // Initialize timing variables
  _lastAddressUpdate = 0;
  _lastDataUpdate = 0;
  _currentAddressValue = 0;
  _currentDataValue = 0;

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const char *menuItems[] = {(const char *)F("Test Signal"),   (const char *)F("Address"),
                             (const char *)F("Address Count"), (const char *)F("Data"),
                             (const char *)F("Data Count"),    (const char *)F("RAS Signal"),
                             (const char *)F("CAS Signal"),    (const char *)F("MUX Signal"),
                             (const char *)F("Read Signal"),   (const char *)F("Write Signal"),
                             (const char *)F("In Signal"),     (const char *)F("Out Signal"),
                             (const char *)F("Wait Signal"),   (const char *)F("Interrupt Signal")};
  setMenuItems(menuItems, 14);

  Globals.logger.info(F("Advanced Menu initialized"));
}

bool AdvancedMenu::open() {
  // Stop the Model1 instance from controlling the signals
  Model1.end();

  // Call parent implementation
  return MenuScreen::open();
}

void AdvancedMenu::close() {
  // Call parent implementation
  MenuScreen::close();

  // Restart Model1 to allow it to control signals again
  Model1.begin();
}

void AdvancedMenu::loop() {
  // Call parent loop first
  MenuScreen::loop();

  // Only process timing and apply signals if test signal is active
  if (!_testSignalActive) {
    return;
  }

  unsigned long currentTime = millis();
  bool updateNeeded = false;

  // Handle address pattern cycling if in Count mode
  if (_addressMode == 4) {  // Count mode
    unsigned long addressInterval = _getDurationMs(_addressCountDuration);
    if (currentTime - _lastAddressUpdate >= addressInterval) {
      _currentAddressValue = _getPatternValue16Bit(_addressMode, _currentAddressValue);
      _lastAddressUpdate = currentTime;
      updateNeeded = true;
    }
  } else {
    // Static address pattern
    uint8_t newAddressValue = _getPatternValue16Bit(_addressMode, 0);
    if (_currentAddressValue != newAddressValue) {
      _currentAddressValue = newAddressValue;
      updateNeeded = true;
    }
  }

  // Handle data pattern cycling if in Count mode
  if (_dataMode == 4) {  // Count mode
    unsigned long dataInterval = _getDurationMs(_dataCountDuration);
    if (currentTime - _lastDataUpdate >= dataInterval) {
      _currentDataValue = _getPatternValue8Bit(_dataMode, _currentDataValue);
      _lastDataUpdate = currentTime;
      updateNeeded = true;
    }
  } else {
    // Static data pattern
    uint8_t newDataValue = _getPatternValue8Bit(_dataMode, 0);
    if (_currentDataValue != newDataValue) {
      _currentDataValue = newDataValue;
      updateNeeded = true;
    }
  }

  // Apply all signal states to Model1 if any changes occurred
  if (updateNeeded || _lastAddressUpdate == 0) {  // Also apply on first run
    _applySignalsToModel1();
  }
}

Screen *AdvancedMenu::_getSelectedMenuItemScreen(int index) {
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

    case -1:  // Back to Main
      Globals.logger.info(F("Returning to main menu from Advanced Menu"));
      return new MainMenu();

    default:
      return nullptr;
  }
}

const char *AdvancedMenu::_getMenuItemConfigValue(uint8_t index) {
  switch (index) {
    case 0:  // Test Signal
      return _testSignalActive ? (const char *)F("On") : (const char *)F("Off");
    case 1:  // Address
      switch (_addressMode) {
        case 0:
          return (const char *)F("0x00");
        case 1:
          return (const char *)F("0x55");
        case 2:
          return (const char *)F("0xAA");
        case 3:
          return (const char *)F("0xFF");
        case 4:
          return (const char *)F("Count");
        default:
          return (const char *)F("0x00");
      }
    case 2:  // Address Count
      if (_addressMode != 4)
        return (const char *)F("Off");  // Only active when Address is "Count"
      switch (_addressCountDuration) {
        case 0:
          return (const char *)F("1s");
        case 1:
          return (const char *)F("5s");
        case 2:
          return (const char *)F("10s");
        case 3:
          return (const char *)F("30s");
        case 4:
          return (const char *)F("60s");
        default:
          return (const char *)F("1s");
      }
    case 3:  // Data
      switch (_dataMode) {
        case 0:
          return (const char *)F("0x00");
        case 1:
          return (const char *)F("0x55");
        case 2:
          return (const char *)F("0xAA");
        case 3:
          return (const char *)F("0xFF");
        case 4:
          return (const char *)F("Count");
        default:
          return (const char *)F("0x00");
      }
    case 4:  // Data Count
      if (_dataMode != 4)
        return (const char *)F("Off");  // Only active when Data is "Count"
      switch (_dataCountDuration) {
        case 0:
          return (const char *)F("1s");
        case 1:
          return (const char *)F("5s");
        case 2:
          return (const char *)F("10s");
        case 3:
          return (const char *)F("30s");
        case 4:
          return (const char *)F("60s");
        default:
          return (const char *)F("1s");
      }
    case 5:  // RAS Signal
      return _rasSignal ? (const char *)F("On") : (const char *)F("Off");
    case 6:  // CAS Signal
      return _casSignal ? (const char *)F("On") : (const char *)F("Off");
    case 7:  // MUX Signal
      return _muxSignal ? (const char *)F("On") : (const char *)F("Off");
    case 8:  // Read Signal
      return _readSignal ? (const char *)F("On") : (const char *)F("Off");
    case 9:  // Write Signal
      return _writeSignal ? (const char *)F("On") : (const char *)F("Off");
    case 10:  // In Signal
      return _inSignal ? (const char *)F("On") : (const char *)F("Off");
    case 11:  // Out Signal
      return _outSignal ? (const char *)F("On") : (const char *)F("Off");
    case 12:  // Wait Signal
      return _waitSignal ? (const char *)F("On") : (const char *)F("Off");
    case 13:  // Interrupt Signal
      return _interruptSignal ? (const char *)F("On") : (const char *)F("Off");
    default:
      return nullptr;
  }
}

bool AdvancedMenu::_isMenuItemEnabled(uint8_t index) {
  switch (index) {
    case 0:   // Test Signal - always enabled
    case 12:  // Wait Signal - always enabled
    case 13:  // Interrupt Signal - always enabled
      return true;
    default:
      // All other items require test signal to be active
      return _testSignalActive;
  }
}

// Toggle methods implementation
void AdvancedMenu::_toggleAddressMode() {
  _addressMode = (_addressMode + 1) % 5;  // Cycle through 0-4 (0x00, 0x55, 0xAA, 0xFF, Count)
  Globals.logger.info((const char *)F("Address mode toggled to %d"), _addressMode);
}

void AdvancedMenu::_toggleAddressCountDuration() {
  _addressCountDuration =
      (_addressCountDuration + 1) % 5;  // Cycle through 0-4 (1s, 5s, 10s, 30s, 60s)
  Globals.logger.info((const char *)F("Address count duration toggled to %d"),
                      _addressCountDuration);
}

void AdvancedMenu::_toggleDataMode() {
  _dataMode = (_dataMode + 1) % 5;  // Cycle through 0-4 (0x00, 0x55, 0xAA, 0xFF, Count)
  Globals.logger.info((const char *)F("Data mode toggled to %d"), _dataMode);
}

void AdvancedMenu::_toggleDataCountDuration() {
  _dataCountDuration = (_dataCountDuration + 1) % 5;  // Cycle through 0-4 (1s, 5s, 10s, 30s, 60s)
  Globals.logger.info((const char *)F("Data count duration toggled to %d"), _dataCountDuration);
}

void AdvancedMenu::_toggleRasSignal() {
  _rasSignal = !_rasSignal;
  Globals.logger.info(_rasSignal ? F("RAS Signal enabled") : F("RAS Signal disabled"));
}

void AdvancedMenu::_toggleCasSignal() {
  _casSignal = !_casSignal;
  Globals.logger.info(_casSignal ? F("CAS Signal enabled") : F("CAS Signal disabled"));
}

void AdvancedMenu::_toggleMuxSignal() {
  _muxSignal = !_muxSignal;
  Globals.logger.info(_muxSignal ? F("MUX Signal enabled") : F("MUX Signal disabled"));
}

void AdvancedMenu::_toggleReadSignal() {
  _readSignal = !_readSignal;
  Globals.logger.info(_readSignal ? F("Read Signal enabled") : F("Read Signal disabled"));
}

void AdvancedMenu::_toggleWriteSignal() {
  _writeSignal = !_writeSignal;
  Globals.logger.info(_writeSignal ? F("Write Signal enabled") : F("Write Signal disabled"));
}

void AdvancedMenu::_toggleInSignal() {
  _inSignal = !_inSignal;
  Globals.logger.info(_inSignal ? F("In Signal enabled") : F("In Signal disabled"));
}

void AdvancedMenu::_toggleOutSignal() {
  _outSignal = !_outSignal;
  Globals.logger.info(_outSignal ? F("Out Signal enabled") : F("Out Signal disabled"));
}

void AdvancedMenu::_toggleWaitSignal() {
  _waitSignal = !_waitSignal;
  Globals.logger.info(_waitSignal ? F("Wait Signal enabled") : F("Wait Signal disabled"));
}

void AdvancedMenu::_toggleInterruptSignal() {
  _interruptSignal = !_interruptSignal;
  Globals.logger.info(_interruptSignal ? F("Interrupt Signal enabled")
                                       : F("Interrupt Signal disabled"));
}

void AdvancedMenu::_toggleTestSignal() {
  _testSignalActive = !_testSignalActive;

  if (_testSignalActive) {
    Model1LowLevel::writeTEST(LOW);
    Globals.logger.info(F("Test signal activated - hardware controls enabled"));
    // Reset timing when activating
    _lastAddressUpdate = 0;
    _lastDataUpdate = 0;
  } else {
    Model1LowLevel::writeTEST(HIGH);
    Globals.logger.info(F("Test signal deactivated - only Wait and Interrupt signals available"));
  }
}

// Helper method implementations
unsigned long AdvancedMenu::_getDurationMs(uint8_t durationIndex) {
  switch (durationIndex) {
    case 0:
      return 1000;  // 1s
    case 1:
      return 5000;  // 5s
    case 2:
      return 10000;  // 10s
    case 3:
      return 30000;  // 30s
    case 4:
      return 60000;  // 60s
    default:
      return 1000;
  }
}

uint8_t AdvancedMenu::_getPatternValue8Bit(uint8_t mode, uint8_t currentValue) {
  switch (mode) {
    case 0:
      return 0x00;
    case 1:
      return 0x55;
    case 2:
      return 0xAA;
    case 3:
      return 0xFF;
    case 4:                     // Count mode - increment from 0x00 to 0xFF, then wrap to 0x00
      return currentValue + 1;  // uint8_t will automatically wrap from 0xFF to 0x00
    default:
      return 0x00;
  }
}

uint16_t AdvancedMenu::_getPatternValue16Bit(uint8_t mode, uint16_t currentValue) {
  switch (mode) {
    case 0:
      return 0x0000;
    case 1:
      return 0x5555;
    case 2:
      return 0xAAAA;
    case 3:
      return 0xFFFF;
    case 4:                     // Count mode - increment from 0x0000 to 0xFFFF, then wrap to 0x0000
      return currentValue + 1;  // uint16_t will automatically wrap from 0xFFFF to 0x0000
    default:
      return 0x0000;
  }
}

void AdvancedMenu::_applySignalsToModel1() {
  // Apply address and data values
  Model1LowLevel::writeAddressBus(_currentAddressValue);
  Model1LowLevel::writeDataBus(_currentDataValue);

  // Apply signal states
  Model1LowLevel::writeRAS(_rasSignal);
  Model1LowLevel::writeCAS(_casSignal);
  Model1LowLevel::writeMUX(_muxSignal);
  Model1LowLevel::writeRD(_readSignal);
  Model1LowLevel::writeWR(_writeSignal);
  Model1LowLevel::writeIN(_inSignal);
  Model1LowLevel::writeOUT(_outSignal);
  Model1LowLevel::writeWAIT(_waitSignal);
  Model1LowLevel::writeINT(_interruptSignal);

  // For now, just log the applied values for debugging
  Globals.logger.info(F("Applied signals to Model1"));
}
