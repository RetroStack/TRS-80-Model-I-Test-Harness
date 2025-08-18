#include "./CassetteMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./CassetteSongPlayerMenu.h"
#include "./CassetteTestSuiteConsole.h"

CassetteMenu::CassetteMenu() : MenuScreen() {
  setTitleF(F("Cassette"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const __FlashStringHelper *menuItems[] = {F("Test Suite"), F("Song Player"), F("Remote Control")};
  setMenuItemsF(menuItems, 3);

  _remoteActive = false;  // Initialize remote as inactive

}

void CassetteMenu::close() {
  // Deactivate remote if it's currently active when leaving the menu
  if (_remoteActive) {
    Globals.logger.infoF(F("Deactivating remote control before leaving cassette menu..."));

    // Briefly activate test signal for the deactivation operation
    Model1.activateTestSignal();
    Globals.cassette.deactivateRemote();
    _remoteActive = false;
    Model1.deactivateTestSignal();

    Globals.logger.infoF(F("Remote control deactivated"));
  }

  Globals.logger.infoF(F("Cassette Interface closed"));

  Screen::close();
}

Screen *CassetteMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Test Suite
      return new CassetteTestSuiteConsole();

    case 1:  // Song Player
      return new CassetteSongPlayerMenu();

    case 2:  // Remote Control (toggle)
      toggleRemote();
      return nullptr;  // Stay on this screen

    case -1:  // Back to Main
      return new MainMenu();

    default:
      return nullptr;
  }
}

const __FlashStringHelper *CassetteMenu::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 2:  // Remote Control
      return _remoteActive ? F("Active") : F("Inactive");
    default:
      return nullptr;
  }
}

void CassetteMenu::toggleRemote() {
  // Briefly activate test signal for cassette operations
  Model1.activateTestSignal();
  Globals.logger.infoF(F("Test signal activated for remote toggle"));

  if (_remoteActive) {
    Globals.logger.infoF(F("Deactivating cassette remote control..."));
    Globals.cassette.deactivateRemote();
    _remoteActive = false;
    Globals.logger.infoF(F("Cassette remote control deactivated"));
  } else {
    Globals.logger.infoF(F("Activating cassette remote control..."));
    Globals.cassette.activateRemote();
    _remoteActive = true;
    Globals.logger.infoF(F("Cassette remote control activated"));
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.infoF(F("Test signal deactivated after remote toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}
