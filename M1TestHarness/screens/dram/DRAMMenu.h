#ifndef DRAM_MENU_H
#define DRAM_MENU_H

#include <MenuScreen.h>

class DRAMMenu : public MenuScreen {
 public:
  DRAMMenu();

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const char *_getMenuItemConfigValue(uint8_t index) override;
};

#endif  // DRAM_MENU_SCREEN_H
