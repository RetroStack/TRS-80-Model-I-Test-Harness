#ifndef CASSETTE_MENU_H
#define CASSETTE_MENU_H

#include <Cassette.h>
#include <MenuScreen.h>

class CassetteMenu : public MenuScreen {
 private:
  bool _remoteActive;

 public:
  CassetteMenu();
  void close() override;

 protected:
  Screen *_getSelectedMenuItemScreen(int index) override;
  const __FlashStringHelper *_getMenuItemConfigValueF(uint8_t index) override;

 private:
  void toggleRemote();
};

#endif  // CASSETTE_MENU_H
