#include "./AdvancedSignalController.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"

// Global instance
AdvancedSignalController& AdvancedSignals = AdvancedSignalController::getInstance();

AdvancedSignalController::AdvancedSignalController() {
  // Initialize state variables
  _isActive = false;
  _testSignalActive = false;
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

  // Initialize timing variables
  _lastAddressUpdate = 0;
  _lastDataUpdate = 0;
  _currentAddressValue = 0;
  _currentDataValue = 0;
}

void AdvancedSignalController::begin() {
  if (!_isActive) {
    // Stop the Model1 instance from controlling the signals
    Model1.end();
    _isActive = true;

    // Reset timing when starting
    _lastAddressUpdate = 0;
    _lastDataUpdate = 0;

    Globals.logger.infoF(F("AdvancedSignalController started"));
  }
}

void AdvancedSignalController::end() {
  if (_isActive) {
    _isActive = false;

    // Restart Model1 to allow it to control signals again
    Model1.begin();

    Globals.logger.infoF(F("AdvancedSignalController stopped"));
  }
}

void AdvancedSignalController::loop() {
  // Only process if controller is active and test signal is active
  if (!_isActive || !_testSignalActive) {
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
    uint16_t newAddressValue = _getPatternValue16Bit(_addressMode, 0);
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

// Configuration setters
void AdvancedSignalController::setTestSignalActive(bool active) {
  _testSignalActive = active;

  if (_testSignalActive) {
    Model1LowLevel::writeTEST(LOW);
    Globals.logger.infoF(F("Test signal activated - hardware controls enabled"));
    // Reset timing when activating
    _lastAddressUpdate = 0;
    _lastDataUpdate = 0;
  } else {
    Model1LowLevel::writeTEST(HIGH);
    Globals.logger.infoF(F("Test signal deactivated - only Wait and Interrupt signals available"));
  }
}

void AdvancedSignalController::setAddressMode(uint8_t mode) {
  _addressMode = mode;
  Globals.logger.infoF(F("Address mode set to %d"), _addressMode);
}

void AdvancedSignalController::setAddressCountDuration(uint8_t duration) {
  _addressCountDuration = duration;
  Globals.logger.infoF(F("Address count duration set to %d"), _addressCountDuration);
}

void AdvancedSignalController::setDataMode(uint8_t mode) {
  _dataMode = mode;
  Globals.logger.infoF(F("Data mode set to %d"), _dataMode);
}

void AdvancedSignalController::setDataCountDuration(uint8_t duration) {
  _dataCountDuration = duration;
  Globals.logger.infoF(F("Data count duration set to %d"), _dataCountDuration);
}

void AdvancedSignalController::setRasSignal(bool state) {
  _rasSignal = state;
  Globals.logger.infoF(_rasSignal ? F("RAS Signal enabled") : F("RAS Signal disabled"));
}

void AdvancedSignalController::setCasSignal(bool state) {
  _casSignal = state;
  Globals.logger.infoF(_casSignal ? F("CAS Signal enabled") : F("CAS Signal disabled"));
}

void AdvancedSignalController::setMuxSignal(bool state) {
  _muxSignal = state;
  Globals.logger.infoF(_muxSignal ? F("MUX Signal enabled") : F("MUX Signal disabled"));
}

void AdvancedSignalController::setReadSignal(bool state) {
  _readSignal = state;
  Globals.logger.infoF(_readSignal ? F("Read Signal enabled") : F("Read Signal disabled"));
}

void AdvancedSignalController::setWriteSignal(bool state) {
  _writeSignal = state;
  Globals.logger.infoF(_writeSignal ? F("Write Signal enabled") : F("Write Signal disabled"));
}

void AdvancedSignalController::setInSignal(bool state) {
  _inSignal = state;
  Globals.logger.infoF(_inSignal ? F("In Signal enabled") : F("In Signal disabled"));
}

void AdvancedSignalController::setOutSignal(bool state) {
  _outSignal = state;
  Globals.logger.infoF(_outSignal ? F("Out Signal enabled") : F("Out Signal disabled"));
}

void AdvancedSignalController::setWaitSignal(bool state) {
  _waitSignal = state;
  Globals.logger.infoF(_waitSignal ? F("Wait Signal enabled") : F("Wait Signal disabled"));
}

void AdvancedSignalController::setInterruptSignal(bool state) {
  _interruptSignal = state;
  Globals.logger.infoF(_interruptSignal ? F("Interrupt Signal enabled")
                                        : F("Interrupt Signal disabled"));
}

// Toggle methods
void AdvancedSignalController::toggleTestSignal() {
  setTestSignalActive(!_testSignalActive);
}

void AdvancedSignalController::toggleAddressMode() {
  setAddressMode((_addressMode + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleAddressCountDuration() {
  setAddressCountDuration((_addressCountDuration + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleDataMode() {
  setDataMode((_dataMode + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleDataCountDuration() {
  setDataCountDuration((_dataCountDuration + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleRasSignal() {
  setRasSignal(!_rasSignal);
}

void AdvancedSignalController::toggleCasSignal() {
  setCasSignal(!_casSignal);
}

void AdvancedSignalController::toggleMuxSignal() {
  setMuxSignal(!_muxSignal);
}

void AdvancedSignalController::toggleReadSignal() {
  setReadSignal(!_readSignal);
}

void AdvancedSignalController::toggleWriteSignal() {
  setWriteSignal(!_writeSignal);
}

void AdvancedSignalController::toggleInSignal() {
  setInSignal(!_inSignal);
}

void AdvancedSignalController::toggleOutSignal() {
  setOutSignal(!_outSignal);
}

void AdvancedSignalController::toggleWaitSignal() {
  setWaitSignal(!_waitSignal);
}

void AdvancedSignalController::toggleInterruptSignal() {
  setInterruptSignal(!_interruptSignal);
}

// Helper methods for menu display
const __FlashStringHelper* AdvancedSignalController::getAddressModeString() const {
  switch (_addressMode) {
    case 0:
      return F("0x00");
    case 1:
      return F("0x55");
    case 2:
      return F("0xAA");
    case 3:
      return F("0xFF");
    case 4:
      return F("Count");
    default:
      return F("0x00");
  }
}

const __FlashStringHelper* AdvancedSignalController::getAddressCountDurationString() const {
  if (_addressMode != 4)
    return F("Off");  // Only active when Address is "Count"

  switch (_addressCountDuration) {
    case 0:
      return F("1s");
    case 1:
      return F("5s");
    case 2:
      return F("10s");
    case 3:
      return F("30s");
    case 4:
      return F("60s");
    default:
      return F("1s");
  }
}

const __FlashStringHelper* AdvancedSignalController::getDataModeString() const {
  switch (_dataMode) {
    case 0:
      return F("0x00");
    case 1:
      return F("0x55");
    case 2:
      return F("0xAA");
    case 3:
      return F("0xFF");
    case 4:
      return F("Count");
    default:
      return F("0x00");
  }
}

const __FlashStringHelper* AdvancedSignalController::getDataCountDurationString() const {
  if (_dataMode != 4)
    return F("Off");  // Only active when Data is "Count"

  switch (_dataCountDuration) {
    case 0:
      return F("1s");
    case 1:
      return F("5s");
    case 2:
      return F("10s");
    case 3:
      return F("30s");
    case 4:
      return F("60s");
    default:
      return F("1s");
  }
}

// Private helper methods
unsigned long AdvancedSignalController::_getDurationMs(uint8_t durationIndex) {
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

uint8_t AdvancedSignalController::_getPatternValue8Bit(uint8_t mode, uint8_t currentValue) {
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

uint16_t AdvancedSignalController::_getPatternValue16Bit(uint8_t mode, uint16_t currentValue) {
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

void AdvancedSignalController::_applySignalsToModel1() {
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
}
