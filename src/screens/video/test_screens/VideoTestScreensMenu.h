#ifndef VIDEO_TEST_SCREENS_MENU_H
#define VIDEO_TEST_SCREENS_MENU_H

#include <MenuScreen.h>

class VideoTestScreensMenu : public MenuScreen {
 private:
  int _currentTestScreen;

 public:
  VideoTestScreensMenu();

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const char *_getMenuItemConfigValue(uint8_t index) override;
};

#endif  // VIDEO_TEST_SCREENS_MENU_H
