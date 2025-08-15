#include "./CassetteMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./CassetteSongPlayerMenu.h"

CassetteMenu::CassetteMenu() : MenuScreen() {
  setTitle((const char *)F("Cassette Interface"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const char *menuItems[] = {(const char *)F("Test Suite"), (const char *)F("Song Player"),
                             (const char *)F("Remote Control")};
  setMenuItems(menuItems, 2);

  _remoteActive = false;  // Initialize remote as inactive

  Globals.logger.info(F("Cassette Menu initialized"));
}

void CassetteMenu::close() {
  // Deactivate remote if it's currently active when leaving the menu
  if (_remoteActive) {
    Globals.logger.info(F("Deactivating remote control before leaving cassette menu..."));

    // Briefly activate test signal for the deactivation operation
    Model1.activateTestSignal();
    Globals.cassette.deactivateRemote();
    _remoteActive = false;
    Model1.deactivateTestSignal();

    Globals.logger.info(F("Remote control deactivated"));
  }

  Globals.logger.info(F("Cassette Interface closed"));

  Screen::close();
}

Screen *CassetteMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Song Player
      Globals.logger.info(F("Opening Song Player"));
      return new CassetteSongPlayerMenu();

    case 1:  // Remote Control (toggle)
      toggleRemote();
      return nullptr;  // Stay on this screen

    case -1:  // Back to Main
      Globals.logger.info(F("Returning to main menu from Cassette Menu"));
      return new MainMenu();

    default:
      return nullptr;
  }
}

const char *CassetteMenu::_getMenuItemConfigValue(uint8_t index) {
  switch (index) {
    case 1:  // Remote Control
      return _remoteActive ? (const char *)F("Active") : (const char *)F("Inactive");
    default:
      return nullptr;
  }
}

void CassetteMenu::toggleRemote() {
  // Briefly activate test signal for cassette operations
  Model1.activateTestSignal();
  Globals.logger.info(F("Test signal activated for remote toggle"));

  if (_remoteActive) {
    Globals.logger.info(F("Deactivating cassette remote control..."));
    Globals.cassette.deactivateRemote();
    _remoteActive = false;
    Globals.logger.info(F("Cassette remote control deactivated"));
  } else {
    Globals.logger.info(F("Activating cassette remote control..."));
    Globals.cassette.activateRemote();
    _remoteActive = true;
    Globals.logger.info(F("Cassette remote control activated"));
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.info(F("Test signal deactivated after remote toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}
