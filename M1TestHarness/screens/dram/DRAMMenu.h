#ifndef DRAM_MENU_H
#define DRAM_MENU_H

#include <MenuScreen.h>

class DRAMMenu : public MenuScreen {
 private:
  uint16_t _currentDRAMSizeKB;  // Track current DRAM size

 public:
  DRAMMenu();
  bool open() override;

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const char *_getMenuItemConfigValue(uint8_t index) override;

 private:
  void toggleDRAMSize();
  static char _configBuffer[16];  // Static buffer for dynamic string formatting
};

#endif  // DRAM_MENU_SCREEN_H
