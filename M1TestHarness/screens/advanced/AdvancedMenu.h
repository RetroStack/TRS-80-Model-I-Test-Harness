#ifndef ADVANCED_MENU_H
#define ADVANCED_MENU_H

#include <MenuScreen.h>

class AdvancedMenu : public MenuScreen {
 public:
  AdvancedMenu();

  bool open() override;
  void close() override;
  void loop() override;

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const char *_getMenuItemConfigValue(uint8_t index) override;
  bool _isMenuItemEnabled(uint8_t index);

 private:
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
  bool _testSignalActive;

  // Timing control
  unsigned long _lastAddressUpdate;
  unsigned long _lastDataUpdate;
  uint16_t _currentAddressValue;
  uint8_t _currentDataValue;

  // Helper methods
  void _toggleAddressMode();
  void _toggleAddressCountDuration();
  void _toggleDataMode();
  void _toggleDataCountDuration();
  void _toggleRasSignal();
  void _toggleCasSignal();
  void _toggleMuxSignal();
  void _toggleReadSignal();
  void _toggleWriteSignal();
  void _toggleInSignal();
  void _toggleOutSignal();
  void _toggleWaitSignal();
  void _toggleInterruptSignal();
  void _toggleTestSignal();

  // Timing and application methods
  unsigned long _getDurationMs(uint8_t durationIndex);
  uint8_t _getPatternValue8Bit(uint8_t mode, uint8_t currentValue);
  uint16_t _getPatternValue16Bit(uint8_t mode, uint16_t currentValue);
  void _applySignalsToModel1();
};

#endif  // ADVANCED_MENU_H
