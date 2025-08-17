#ifndef VIDEO_MENU_H
#define VIDEO_MENU_H

#include <MenuScreen.h>

class VideoMenu : public MenuScreen {
 private:
  uint8_t _charGen;        // Track current character generator (0 = unknown, 1 = A, 2 = B)
  bool _is64CharMode;      // Track current character mode state
  bool _hasLowerCaseMod;   // Track current lower-case mod state

 public:
  VideoMenu();
  bool open() override;

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const __FlashStringHelper *_getMenuItemConfigValueF(uint8_t index) override;

 private:
  void toggleCharacterMode();
  void toggleCharacterGen();
  void toggleLowerCaseMod();
};

#endif  // VIDEO_MENU_H
