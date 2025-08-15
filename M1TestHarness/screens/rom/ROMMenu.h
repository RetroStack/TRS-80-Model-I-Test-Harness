#ifndef ROM_MENU_H
#define ROM_MENU_H

#include <MenuScreen.h>

class ROMMenu : public MenuScreen {
 public:
  ROMMenu();

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
};

#endif  // ROM_MENU_H
