#include "./VideoMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "./M1Terminal.h"
#include "./VRAMContentViewerConsole.h"
#include "./VRAMTestSuiteConsole.h"
#include "./test_screens/VideoTestScreensMenu.h"

VideoMenu::VideoMenu() : MenuScreen() {
  setTitleF(F("Video"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const __FlashStringHelper *menuItems[] = {
      F("Mirror"),        F("VRAM Viewer"),    F("VRAM Test Suite"), F("Character Mode"),
      F("Character Gen"), F("Lower-case Mod"), F("Test Screens")};
  setMenuItemsF(menuItems, 7);

  _charGen = 0;  // Initialize to "unknown"; can't be determined
  _is64CharMode =
      true;  // Initialize to 64-character mode (default assumption; will be set when opened)
  _hasLowerCaseMod = false;  // Initialize to false; will be read from globals when opened
}

bool VideoMenu::open() {
  // Activate test signal to read initial hardware state
  Model1.activateTestSignal();

  // Read and store the current character mode state
  _is64CharMode = Globals.cassette.is64CharacterMode();

  // Read and store the current lower-case mod state from globals
  _hasLowerCaseMod = Globals.getHasLowerCaseMod();

  // Deactivate test signal after reading
  Model1.deactivateTestSignal();

  // Call parent implementation
  return MenuScreen::open();
}

Screen *VideoMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Mirror->M1Terminal (toggle)
      return new M1Terminal();

    case 1:  // VRAM Viewer
      return new VRAMContentViewerConsole();

    case 2:  // VRAM Test Suite
      return new VRAMTestSuiteConsole();

    case 3:  // Character Mode (toggle 64/32)
      toggleCharacterMode();
      return nullptr;  // Stay on this screen

    case 4:  // Character Gen (toggle A/B)
      toggleCharacterGen();
      return nullptr;  // Stay on this screen

    case 5:  // Lower-case Mod (toggle)
      toggleLowerCaseMod();
      return nullptr;  // Stay on this screen

    case 6:  // Test Screens
      return new VideoTestScreensMenu();

    case -1:  // Back to Main
      return new MainMenu();

    default:
      return nullptr;
  }
}

const __FlashStringHelper *VideoMenu::_getMenuItemConfigValueF(uint8_t index) {
  switch (index) {
    case 3:  // Character Mode
      return _is64CharMode ? F("64 chars") : F("32 chars");
    case 4:  // Character Gen
      if (_charGen == 0) {
        return F("Unknown");  // If we don't know the state yet
      } else if (_charGen == 1) {
        return F("Gen A");
      } else {
        return F("Gen B");
      }
    case 5:  // Lower-case Mod
      return _hasLowerCaseMod ? F("Enabled") : F("Disabled");
  }
  return nullptr;  // No config value for other indices
}

void VideoMenu::toggleCharacterMode() {
  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.infoF(F("Test signal activated for character mode toggle"));

  // Get latest state
  _is64CharMode = Globals.cassette.is64CharacterMode();

  if (_is64CharMode) {
    Globals.logger.infoF(F("Switching to 32-character mode..."));
    Globals.cassette.set32CharacterMode();
    Globals.logger.infoF(F("Switched to 32-character mode"));
  } else {
    Globals.logger.infoF(F("Switching to 64-character mode..."));
    Globals.cassette.set64CharacterMode();
    Globals.logger.infoF(F("Switched to 64-character mode"));
  }

  // Get latest state
  _is64CharMode = Globals.cassette.is64CharacterMode();

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.infoF(F("Test signal deactivated after character mode toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}

void VideoMenu::toggleCharacterGen() {
  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.infoF(F("Test signal activated for character generator toggle"));

  if (_charGen < 1) {
    Globals.logger.infoF(F("Switching to Character Generator A..."));
    Globals.cassette.setCharGenA();
    _charGen = 1;
    Globals.logger.infoF(F("Character Generator A selected"));
  } else {
    Globals.logger.infoF(F("Switching to Character Generator B..."));
    Globals.cassette.setCharGenB();
    _charGen = 2;
    Globals.logger.infoF(F("Character Generator B selected"));
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.infoF(F("Test signal deactivated after character generator toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}

void VideoMenu::toggleLowerCaseMod() {
  // Get current state from globals
  _hasLowerCaseMod = Globals.getHasLowerCaseMod();

  // Toggle the state
  _hasLowerCaseMod = !_hasLowerCaseMod;

  // Update globals with new state
  Globals.setHasLowerCaseMod(_hasLowerCaseMod);

  // Log the change
  Globals.logger.info(_hasLowerCaseMod ? F("Lower-case mod toggled: Enabled")
                                       : F("Lower-case mod toggled: Disabled"));

  // Redraw the menu to show updated config value
  _drawContent();
}
