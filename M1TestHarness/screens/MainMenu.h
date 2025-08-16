#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <MenuScreen.h>

class MainMenu : public MenuScreen {
 public:
  MainMenu();

 protected:
  Screen* _getSelectedMenuItemScreen(int index) override;
  const __FlashStringHelper* _getMenuItemConfigValueF(uint8_t index) override;
  bool _isMenuItemEnabled(uint8_t index) const override;
};

#endif  // MAIN_SCREEN_H
