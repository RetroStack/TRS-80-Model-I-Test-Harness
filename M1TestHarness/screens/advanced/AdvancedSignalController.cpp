#include "./AdvancedSignalController.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"

// Global instance
AdvancedSignalController AdvancedSignals;

AdvancedSignalController::AdvancedSignalController() {
  // Initialize state variables
  _isActive = false;
  _testSignalActive = false;
  _addressMode = 0;           // Default to 0x00
  _addressCountDuration = 0;  // Default to 1s
  _dataMode = 0;              // Default to 0x00
  _dataCountDuration = 0;     // Default to 1s

  // Initialize individual signal modes - default to floating (0) when test signal is active
  _rasSignalMode = 0;  // 0=floating, 1=off/low, 2=on/high
  _casSignalMode = 0;
  _muxSignalMode = 0;
  _readSignalMode = 0;
  _writeSignalMode = 0;
  _inSignalMode = 0;
  _outSignalMode = 0;
  _waitSignalMode = 0;
  _interruptSignalMode = 0;

  // Initialize signal direction controls - default to read mode (floating) when test signal is
  // active
  _addressBusWriteMode = false;  // Start in read/floating mode for input monitoring
  _dataBusWriteMode = false;     // Start in read/floating mode for input monitoring

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

  // Address bus: mode 0 = floating, do not drive or update value
  if (_addressMode == 0) {
    // Set to floating, do not update value
    // (Handled in _applySignalsToModel1)
  } else if (_addressMode == 5) {  // Count mode
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

  // Data bus: mode 0 = floating, do not drive or update value
  if (_dataMode == 0) {
    // Set to floating, do not update value
    // (Handled in _applySignalsToModel1)
  } else if (_dataMode == 5) {  // Count mode
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

// Configuration getters
bool AdvancedSignalController::isTestSignalActive() const {
  return _testSignalActive;
}

uint8_t AdvancedSignalController::getAddressMode() const {
  return _addressMode;
}

uint8_t AdvancedSignalController::getAddressCountDuration() const {
  return _addressCountDuration;
}

uint8_t AdvancedSignalController::getDataMode() const {
  return _dataMode;
}

uint8_t AdvancedSignalController::getDataCountDuration() const {
  return _dataCountDuration;
}

uint8_t AdvancedSignalController::getRasSignalMode() const {
  return _rasSignalMode;
}

uint8_t AdvancedSignalController::getCasSignalMode() const {
  return _casSignalMode;
}

uint8_t AdvancedSignalController::getMuxSignalMode() const {
  return _muxSignalMode;
}

uint8_t AdvancedSignalController::getReadSignalMode() const {
  return _readSignalMode;
}

uint8_t AdvancedSignalController::getWriteSignalMode() const {
  return _writeSignalMode;
}

uint8_t AdvancedSignalController::getInSignalMode() const {
  return _inSignalMode;
}

uint8_t AdvancedSignalController::getOutSignalMode() const {
  return _outSignalMode;
}

uint8_t AdvancedSignalController::getWaitSignalMode() const {
  return _waitSignalMode;
}

uint8_t AdvancedSignalController::getInterruptSignalMode() const {
  return _interruptSignalMode;
}

bool AdvancedSignalController::getAddressBusWriteMode() const {
  return _addressBusWriteMode;
}

bool AdvancedSignalController::getDataBusWriteMode() const {
  return _dataBusWriteMode;
}

// Configuration setters
void AdvancedSignalController::setTestSignalActive(bool active) {
  _testSignalActive = active;

  if (_testSignalActive) {
    Model1LowLevel::writeTEST(LOW);
    Globals.logger.infoF(F("Test signal activated - hardware controls enabled"));

    // Set default signal directions to read mode (floating) for input monitoring
    _addressBusWriteMode = false;
    _dataBusWriteMode = false;

    // Set all individual signals to floating mode (0) by default
    _rasSignalMode = 0;
    _casSignalMode = 0;
    _muxSignalMode = 0;
    _readSignalMode = 0;
    _writeSignalMode = 0;
    _inSignalMode = 0;
    _outSignalMode = 0;
    _waitSignalMode = 0;
    _interruptSignalMode = 0;

    // Configure all signals as inputs (floating) by default
    _configureSignalDirections();

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

void AdvancedSignalController::setRasSignalMode(uint8_t mode) {
  _rasSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("RAS Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("RAS Signal mode: off"));
  } else {
    Globals.logger.infoF(F("RAS Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setCasSignalMode(uint8_t mode) {
  _casSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("CAS Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("CAS Signal mode: off"));
  } else {
    Globals.logger.infoF(F("CAS Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setMuxSignalMode(uint8_t mode) {
  _muxSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("MUX Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("MUX Signal mode: off"));
  } else {
    Globals.logger.infoF(F("MUX Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setReadSignalMode(uint8_t mode) {
  _readSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("Read Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("Read Signal mode: off"));
  } else {
    Globals.logger.infoF(F("Read Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setWriteSignalMode(uint8_t mode) {
  _writeSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("Write Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("Write Signal mode: off"));
  } else {
    Globals.logger.infoF(F("Write Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setInSignalMode(uint8_t mode) {
  _inSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("IN Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("IN Signal mode: off"));
  } else {
    Globals.logger.infoF(F("IN Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setOutSignalMode(uint8_t mode) {
  _outSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("OUT Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("OUT Signal mode: off"));
  } else {
    Globals.logger.infoF(F("OUT Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setWaitSignalMode(uint8_t mode) {
  _waitSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("WAIT Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("WAIT Signal mode: off"));
  } else {
    Globals.logger.infoF(F("WAIT Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

void AdvancedSignalController::setInterruptSignalMode(uint8_t mode) {
  _interruptSignalMode = mode;
  if (mode == 0) {
    Globals.logger.infoF(F("INTERRUPT Signal mode: floating"));
  } else if (mode == 1) {
    Globals.logger.infoF(F("INTERRUPT Signal mode: off"));
  } else {
    Globals.logger.infoF(F("INTERRUPT Signal mode: on"));
  }
  _configureSignalDirections();
  _applySignalsToModel1();
}

// Signal direction setters
void AdvancedSignalController::setAddressBusWriteMode(bool writeMode) {
  _addressBusWriteMode = writeMode;
  _configureSignalDirections();
  Globals.logger.infoF(_addressBusWriteMode ? F("Address bus set to WRITE mode")
                                            : F("Address bus set to READ mode (floating)"));
}

void AdvancedSignalController::setDataBusWriteMode(bool writeMode) {
  _dataBusWriteMode = writeMode;
  _configureSignalDirections();
  Globals.logger.infoF(_dataBusWriteMode ? F("Data bus set to WRITE mode")
                                         : F("Data bus set to read mode (floating)"));
}

// Toggle methods
void AdvancedSignalController::toggleTestSignal() {
  setTestSignalActive(!_testSignalActive);
}

void AdvancedSignalController::toggleAddressMode() {
  setAddressMode((_addressMode + 1) % 6);  // Cycle through 0-5 (now includes floating)
}

void AdvancedSignalController::toggleAddressCountDuration() {
  setAddressCountDuration((_addressCountDuration + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleDataMode() {
  setDataMode((_dataMode + 1) % 6);  // Cycle through 0-5 (now includes floating)
}

void AdvancedSignalController::toggleDataCountDuration() {
  setDataCountDuration((_dataCountDuration + 1) % 5);  // Cycle through 0-4
}

void AdvancedSignalController::toggleRasSignalMode() {
  setRasSignalMode((_rasSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleCasSignalMode() {
  setCasSignalMode((_casSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleMuxSignalMode() {
  setMuxSignalMode((_muxSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleReadSignalMode() {
  setReadSignalMode((_readSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleWriteSignalMode() {
  setWriteSignalMode((_writeSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleInSignalMode() {
  setInSignalMode((_inSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleOutSignalMode() {
  setOutSignalMode((_outSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleWaitSignalMode() {
  setWaitSignalMode((_waitSignalMode + 1) % 3);
}

void AdvancedSignalController::toggleInterruptSignalMode() {
  setInterruptSignalMode((_interruptSignalMode + 1) % 3);
}

// Toggle signal direction methods
void AdvancedSignalController::toggleAddressBusWriteMode() {
  setAddressBusWriteMode(!_addressBusWriteMode);
}

void AdvancedSignalController::toggleDataBusWriteMode() {
  setDataBusWriteMode(!_dataBusWriteMode);
}

// Helper methods for menu display
const __FlashStringHelper* AdvancedSignalController::getAddressModeString() const {
  switch (_addressMode) {
    case 0:
      return F("Floating");
    case 1:
      return F("0x00");
    case 2:
      return F("0x55");
    case 3:
      return F("0xAA");
    case 4:
      return F("0xFF");
    case 5:
      return F("Count");
    default:
      return F("Floating");
  }
}

const __FlashStringHelper* AdvancedSignalController::getAddressCountDurationString() const {
  if (_addressMode != 5)
    return F("Off");  // Only active when Address is "Count" (mode 5)

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
      return F("Floating");
    case 1:
      return F("0x00");
    case 2:
      return F("0x55");
    case 3:
      return F("0xAA");
    case 4:
      return F("0xFF");
    case 5:
      return F("Count");
    default:
      return F("Floating");
  }
}

const __FlashStringHelper* AdvancedSignalController::getDataCountDurationString() const {
  if (_dataMode != 5)
    return F("Off");  // Only active when Data is "Count" (mode 5)

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

// Signal direction display helpers
const __FlashStringHelper* AdvancedSignalController::getAddressBusWriteModeString() const {
  return _addressBusWriteMode ? F("Write") : F("Read");
}

const __FlashStringHelper* AdvancedSignalController::getDataBusWriteModeString() const {
  return _dataBusWriteMode ? F("Write") : F("Read");
}

// Individual signal mode display helpers
const __FlashStringHelper* AdvancedSignalController::getRasSignalModeString() const {
  switch (_rasSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getCasSignalModeString() const {
  switch (_casSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getMuxSignalModeString() const {
  switch (_muxSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getReadSignalModeString() const {
  switch (_readSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getWriteSignalModeString() const {
  switch (_writeSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getInSignalModeString() const {
  switch (_inSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getOutSignalModeString() const {
  switch (_outSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getWaitSignalModeString() const {
  switch (_waitSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
  }
}

const __FlashStringHelper* AdvancedSignalController::getInterruptSignalModeString() const {
  switch (_interruptSignalMode) {
    case 0:
      return F("Float");
    case 1:
      return F("On");
    case 2:
      return F("Off");
    default:
      return F("Float");
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
    case 1:
      return 0x00;
    case 2:
      return 0x55;
    case 3:
      return 0xAA;
    case 4:
      return 0xFF;
    case 5:                     // Count mode - increment from 0x00 to 0xFF, then wrap to 0x00
      return currentValue + 1;  // uint8_t will automatically wrap from 0xFF to 0x00
    default:
      return 0x00;
  }
}

uint16_t AdvancedSignalController::_getPatternValue16Bit(uint8_t mode, uint16_t currentValue) {
  switch (mode) {
    case 1:
      return 0x0000;
    case 2:
      return 0x5555;
    case 3:
      return 0xAAAA;
    case 4:
      return 0xFFFF;
    case 5:                     // Count mode - increment from 0x0000 to 0xFFFF, then wrap to 0x0000
      return currentValue + 1;  // uint16_t will automatically wrap from 0xFFFF to 0x0000
    default:
      return 0x0000;
  }
}

void AdvancedSignalController::_applySignalsToModel1() {
  // Apply address and data values
  Model1LowLevel::writeAddressBus(_currentAddressValue);
  Model1LowLevel::writeDataBus(_currentDataValue);

  // Apply signal states based on mode (0=floating, 1=off/low, 2=on/high)
  // Only apply signals that are not in floating mode
  if (_rasSignalMode != 0) {
    Model1LowLevel::writeRAS(_rasSignalMode == 2);
  }
  if (_casSignalMode != 0) {
    Model1LowLevel::writeCAS(_casSignalMode == 2);
  }
  if (_muxSignalMode != 0) {
    Model1LowLevel::writeMUX(_muxSignalMode == 2);
  }
  if (_readSignalMode != 0) {
    Model1LowLevel::writeRD(_readSignalMode == 2);
  }
  if (_writeSignalMode != 0) {
    Model1LowLevel::writeWR(_writeSignalMode == 2);
  }
  if (_inSignalMode != 0) {
    Model1LowLevel::writeIN(_inSignalMode == 2);
  }
  if (_outSignalMode != 0) {
    Model1LowLevel::writeOUT(_outSignalMode == 2);
  }
  if (_waitSignalMode != 0) {
    Model1LowLevel::writeWAIT(_waitSignalMode == 2);
  }
  if (_interruptSignalMode != 0) {
    Model1LowLevel::writeINT(_interruptSignalMode == 2);
  }
}

void AdvancedSignalController::_configureSignalDirections() {
  // Address bus: floating if mode 0, otherwise output
  if (_addressMode == 0) {
    Model1LowLevel::configWriteAddressBus(0x0000);  // All pins as INPUT (floating)
  } else {
    Model1LowLevel::configWriteAddressBus(0xFFFF);  // All pins as OUTPUT
    Model1LowLevel::writeAddressBus(_currentAddressValue);
  }

  // Data bus: floating if mode 0, otherwise output
  if (_dataMode == 0) {
    Model1LowLevel::configWriteDataBus(0x00);  // All pins as INPUT (floating)
  } else {
    Model1LowLevel::configWriteDataBus(0xFF);  // All pins as OUTPUT
    Model1LowLevel::writeDataBus(_currentDataValue);
  }

  // Configure individual signal directions based on mode
  // 0=floating (INPUT), 1&2=driven (OUTPUT)
  Model1LowLevel::configWriteRAS(_rasSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteCAS(_casSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteMUX(_muxSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteRD(_readSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteWR(_writeSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteIN(_inSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteOUT(_outSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteWAIT(_waitSignalMode == 0 ? INPUT : OUTPUT);
  Model1LowLevel::configWriteINT(_interruptSignalMode == 0 ? INPUT : OUTPUT);
}
