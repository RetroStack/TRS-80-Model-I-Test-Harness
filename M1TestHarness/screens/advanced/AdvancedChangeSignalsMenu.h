#ifndef ADVANCED_CHANGE_SIGNALS_MENU_H
#define ADVANCED_CHANGE_SIGNALS_MENU_H

#include <MenuScreen.h>

#include "./AdvancedSignalController.h"

class AdvancedChangeSignalsMenu : public MenuScreen {
 public:
  AdvancedChangeSignalsMenu();

  bool open() override;
  void close() override;
  void loop() override;

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const __FlashStringHelper *_getMenuItemConfigValueF(uint8_t index) override;
  bool _isMenuItemEnabled(uint8_t index) const override;

 private:
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
};

#endif  // ADVANCED_CHANGE_SIGNALS_MENU_H
