#ifndef ADVANCED_SIGNAL_CONTROLLER_H
#define ADVANCED_SIGNAL_CONTROLLER_H

#include <Arduino.h>

class AdvancedSignalController {
 public:
  // Singleton instance
  static AdvancedSignalController& getInstance() {
    static AdvancedSignalController instance;
    return instance;
  }

  // Initialize the controller
  void begin();

  // Stop the controller
  void end();

  // Main loop function to be called continuously by all Advanced screens
  void loop();

  // Configuration getters
  bool isTestSignalActive() const {
    return _testSignalActive;
  }
  uint8_t getAddressMode() const {
    return _addressMode;
  }
  uint8_t getAddressCountDuration() const {
    return _addressCountDuration;
  }
  uint8_t getDataMode() const {
    return _dataMode;
  }
  uint8_t getDataCountDuration() const {
    return _dataCountDuration;
  }
  bool getRasSignal() const {
    return _rasSignal;
  }
  bool getCasSignal() const {
    return _casSignal;
  }
  bool getMuxSignal() const {
    return _muxSignal;
  }
  bool getReadSignal() const {
    return _readSignal;
  }
  bool getWriteSignal() const {
    return _writeSignal;
  }
  bool getInSignal() const {
    return _inSignal;
  }
  bool getOutSignal() const {
    return _outSignal;
  }
  bool getWaitSignal() const {
    return _waitSignal;
  }
  bool getInterruptSignal() const {
    return _interruptSignal;
  }

  // Configuration setters
  void setTestSignalActive(bool active);
  void setAddressMode(uint8_t mode);
  void setAddressCountDuration(uint8_t duration);
  void setDataMode(uint8_t mode);
  void setDataCountDuration(uint8_t duration);
  void setRasSignal(bool state);
  void setCasSignal(bool state);
  void setMuxSignal(bool state);
  void setReadSignal(bool state);
  void setWriteSignal(bool state);
  void setInSignal(bool state);
  void setOutSignal(bool state);
  void setWaitSignal(bool state);
  void setInterruptSignal(bool state);

  // Toggle methods for easy menu integration
  void toggleTestSignal();
  void toggleAddressMode();
  void toggleAddressCountDuration();
  void toggleDataMode();
  void toggleDataCountDuration();
  void toggleRasSignal();
  void toggleCasSignal();
  void toggleMuxSignal();
  void toggleReadSignal();
  void toggleWriteSignal();
  void toggleInSignal();
  void toggleOutSignal();
  void toggleWaitSignal();
  void toggleInterruptSignal();

  // Helper methods for menu display
  const __FlashStringHelper* getAddressModeString() const;
  const __FlashStringHelper* getAddressCountDurationString() const;
  const __FlashStringHelper* getDataModeString() const;
  const __FlashStringHelper* getDataCountDurationString() const;

 private:
  // Private constructor for singleton
  AdvancedSignalController();

  // Prevent copying
  AdvancedSignalController(const AdvancedSignalController&) = delete;
  AdvancedSignalController& operator=(const AdvancedSignalController&) = delete;

  // Configuration state
  bool _isActive;  // Whether the controller is managing signals
  bool _testSignalActive;

  // Address control
  uint8_t _addressMode;           // 0=0x00, 1=0x55, 2=0xAA, 3=0xFF, 4=Count
  uint8_t _addressCountDuration;  // 0=1s, 1=5s, 2=10s, 3=30s, 4=60s

  // Data control
  uint8_t _dataMode;           // 0=0x00, 1=0x55, 2=0xAA, 3=0xFF, 4=Count
  uint8_t _dataCountDuration;  // 0=1s, 1=5s, 2=10s, 3=30s, 4=60s

  // Signal controls (On/Off)
  bool _rasSignal;
  bool _casSignal;
  bool _muxSignal;
  bool _readSignal;
  bool _writeSignal;
  bool _inSignal;
  bool _outSignal;
  bool _waitSignal;
  bool _interruptSignal;

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
};

// Global instance access
extern AdvancedSignalController& AdvancedSignals;

#endif  // ADVANCED_SIGNAL_CONTROLLER_H
