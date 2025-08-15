#ifndef BOARD_REVISION_MENU_H
#define BOARD_REVISION_MENU_H

#include <MenuScreen.h>

enum class BoardRevisionSource { DIAGNOSTICS, MAIN_MENU };

class BoardRevisionMenu : public MenuScreen {
 public:
  BoardRevisionMenu(BoardRevisionSource source = BoardRevisionSource::MAIN_MENU);

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const char *_getMenuItemConfigValue(uint8_t index) override;

 private:
  BoardRevisionSource _source;
};

#endif  // BOARD_REVISION_MENU_H
