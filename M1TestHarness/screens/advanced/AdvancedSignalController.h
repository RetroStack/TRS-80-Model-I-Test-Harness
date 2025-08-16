#ifndef ADVANCED_SIGNAL_CONTROLLER_H
#define ADVANCED_SIGNAL_CONTROLLER_H

#include <Arduino.h>

class AdvancedSignalController {
 public:
  AdvancedSignalController();

  // Initialize the controller
  void begin();

  // Stop the controller
  void end();

  // Main loop function to be called continuously by all Advanced screens
  void loop();

  // Configuration getters
  bool isTestSignalActive() const;
  uint8_t getAddressMode() const;
  uint8_t getAddressCountDuration() const;
  uint8_t getDataMode() const;
  uint8_t getDataCountDuration() const;
  uint8_t getRasSignalMode() const;
  uint8_t getCasSignalMode() const;
  uint8_t getMuxSignalMode() const;
  uint8_t getReadSignalMode() const;
  uint8_t getWriteSignalMode() const;
  uint8_t getInSignalMode() const;
  uint8_t getOutSignalMode() const;
  uint8_t getWaitSignalMode() const;
  uint8_t getInterruptSignalMode() const;

  // Signal direction getters (true = write/drive mode, false = read/floating mode)
  bool getAddressBusWriteMode() const;
  bool getDataBusWriteMode() const;

  // Configuration setters
  void setTestSignalActive(bool active);
  void setAddressMode(uint8_t mode);
  void setAddressCountDuration(uint8_t duration);
  void setDataMode(uint8_t mode);
  void setDataCountDuration(uint8_t duration);

  // Signal direction setters (true = write/drive mode, false = read/floating mode)
  void setAddressBusWriteMode(bool writeMode);
  void setDataBusWriteMode(bool writeMode);

  // Individual signal mode setters (0=floating, 1=off/low, 2=on/high)
  void setRasSignalMode(uint8_t mode);
  void setCasSignalMode(uint8_t mode);
  void setMuxSignalMode(uint8_t mode);
  void setReadSignalMode(uint8_t mode);
  void setWriteSignalMode(uint8_t mode);
  void setInSignalMode(uint8_t mode);
  void setOutSignalMode(uint8_t mode);
  void setWaitSignalMode(uint8_t mode);
  void setInterruptSignalMode(uint8_t mode);

  // Toggle methods for easy menu integration
  void toggleTestSignal();
  void toggleAddressMode();
  void toggleAddressCountDuration();
  void toggleDataMode();
  void toggleDataCountDuration();

  // Toggle signal direction methods
  void toggleAddressBusWriteMode();
  void toggleDataBusWriteMode();

  // Toggle individual signal modes (0=floating -> 1=off -> 2=on -> 0=floating)
  void toggleRasSignalMode();
  void toggleCasSignalMode();
  void toggleMuxSignalMode();
  void toggleReadSignalMode();
  void toggleWriteSignalMode();
  void toggleInSignalMode();
  void toggleOutSignalMode();
  void toggleWaitSignalMode();
  void toggleInterruptSignalMode();

  // Helper methods for menu display
  const __FlashStringHelper* getAddressModeString() const;
  const __FlashStringHelper* getAddressCountDurationString() const;
  const __FlashStringHelper* getDataModeString() const;
  const __FlashStringHelper* getDataCountDurationString() const;

  // Signal direction display helpers
  const __FlashStringHelper* getAddressBusWriteModeString() const;
  const __FlashStringHelper* getDataBusWriteModeString() const;

  // Individual signal mode display helpers
  const __FlashStringHelper* getRasSignalModeString() const;
  const __FlashStringHelper* getCasSignalModeString() const;
  const __FlashStringHelper* getMuxSignalModeString() const;
  const __FlashStringHelper* getReadSignalModeString() const;
  const __FlashStringHelper* getWriteSignalModeString() const;
  const __FlashStringHelper* getInSignalModeString() const;
  const __FlashStringHelper* getOutSignalModeString() const;
  const __FlashStringHelper* getWaitSignalModeString() const;
  const __FlashStringHelper* getInterruptSignalModeString() const;

 private:
  // Configuration state
  bool _isActive;  // Whether the controller is managing signals
  bool _testSignalActive;

  // Address control
  uint8_t _addressMode;           // 0=0x00, 1=0x55, 2=0xAA, 3=0xFF, 4=Count
  uint8_t _addressCountDuration;  // 0=1s, 1=5s, 2=10s, 3=30s, 4=60s

  // Data control
  uint8_t _dataMode;           // 0=0x00, 1=0x55, 2=0xAA, 3=0xFF, 4=Count
  uint8_t _dataCountDuration;  // 0=1s, 1=5s, 2=10s, 3=30s, 4=60s

  // Signal controls - individual signal modes (0=floating, 1=off/low, 2=on/high)
  uint8_t _rasSignalMode;
  uint8_t _casSignalMode;
  uint8_t _muxSignalMode;
  uint8_t _readSignalMode;
  uint8_t _writeSignalMode;
  uint8_t _inSignalMode;
  uint8_t _outSignalMode;
  uint8_t _waitSignalMode;
  uint8_t _interruptSignalMode;

  // Signal direction controls (true = write/OUTPUT mode, false = read/INPUT mode)
  bool _addressBusWriteMode;  // Default false when test signal active (floating for input
                              // monitoring)
  bool _dataBusWriteMode;  // Default false when test signal active (floating for input monitoring)

  // Timing control
  unsigned long _lastAddressUpdate;
  unsigned long _lastDataUpdate;
  uint16_t _currentAddressValue;
  uint8_t _currentDataValue;

  // Helper methods
  unsigned long _getDurationMs(uint8_t durationIndex);
  uint8_t _getPatternValue8Bit(uint8_t mode, uint8_t currentValue);
  uint16_t _getPatternValue16Bit(uint8_t mode, uint16_t currentValue);
  void _applySignalsToModel1();
  void _configureSignalDirections();
};

// Global instance access
extern AdvancedSignalController AdvancedSignals;

#endif  // ADVANCED_SIGNAL_CONTROLLER_H