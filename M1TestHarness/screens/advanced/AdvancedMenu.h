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
};

#endif  // ADVANCED_MENU_H
