#include "VideoMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"
#include "M1Terminal.h"
#include "VRAMTestSuiteConsole.h"
#include "test_screens/VideoTestScreensMenu.h"

VideoMenu::VideoMenu() : MenuScreen() {
  setTitle((const char *)F("Video"));

  // Create menu items dynamically - they'll be copied by _setMenuItems and these will be freed
  // automatically
  const char *menuItems[] = {(const char *)F("Mirror"), (const char *)F("VRAM Test Suite"),
                             (const char *)F("Character Mode"), (const char *)F("Character Gen"),
                             (const char *)F("Test Screens")};
  setMenuItems(menuItems, 5);

  _charGen = 0;  // Initialize to "unknown"; can't be determined
  _is64CharMode =
      true;  // Initialize to 64-character mode (default assumption; will be set when opened)

  Globals.logger.info(F("Video Menu initialized"));
}

bool VideoMenu::open() {
  // Activate test signal to read initial hardware state
  Model1.activateTestSignal();
  Globals.logger.info(F("Test signal activated for initial state reading"));

  // Read and store the current character mode state
  _is64CharMode = Globals.cassette.is64CharacterMode();
  Globals.logger.info(_is64CharMode ? F("Initial character mode detected: 64 characters")
                                    : F("Initial character mode detected: 32 characters"));

  // Deactivate test signal after reading
  Model1.deactivateTestSignal();
  Globals.logger.info(F("Test signal deactivated after initial state reading"));

  // Call parent implementation
  return MenuScreen::open();
}

Screen *VideoMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Mirror→M1Terminal (toggle)
      Globals.logger.info(F("Opening M1 Terminal Screen"));
      return new M1Terminal();

    case 1:  // VRAM Test Suite
      Globals.logger.info(F("Opening VRAM Test Suite"));
      return new VRAMTestSuiteConsole();

    case 2:  // Character Mode (toggle 64/32)
      toggleCharacterMode();
      return nullptr;  // Stay on this screen

    case 3:  // Character Gen (toggle A/B)
      toggleCharacterGen();
      return nullptr;  // Stay on this screen

    case 4:  // Test Screens
      Globals.logger.info(F("Opening Video Test Screens"));
      return new VideoTestScreensMenu();

    case -1:  // Back to Main
      Globals.logger.info(F("Returning to Main Menu from Video Menu"));
      return new MainMenu();

    default:
      return nullptr;
  }
}

const char *VideoMenu::_getMenuItemConfigValue(uint8_t index) {
  switch (index) {
    case 2:  // Character Mode
      return _is64CharMode ? (const char *)F("64 chars") : (const char *)F("32 chars");
    case 3:  // Character Gen
      if (_charGen == 0) {
        return (const char *)F("Unknown");  // If we don't know the state yet
      } else if (_charGen == 1) {
        return (const char *)F("Gen A");
      } else {
        return (const char *)F("Gen B");
      }
  }
  return nullptr;  // No config value for other indices
}

void VideoMenu::toggleCharacterMode() {
  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.info(F("Test signal activated for character mode toggle"));

  // Get latest state
  _is64CharMode = Globals.cassette.is64CharacterMode();

  if (_is64CharMode) {
    Globals.logger.info(F("Switching to 32-character mode..."));
    Globals.cassette.set32CharacterMode();
    Globals.logger.info(F("Switched to 32-character mode"));
  } else {
    Globals.logger.info(F("Switching to 64-character mode..."));
    Globals.cassette.set64CharacterMode();
    Globals.logger.info(F("Switched to 64-character mode"));
  }

  // Get latest state
  _is64CharMode = Globals.cassette.is64CharacterMode();

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.info(F("Test signal deactivated after character mode toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}

void VideoMenu::toggleCharacterGen() {
  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.info(F("Test signal activated for character generator toggle"));

  if (_charGen < 1) {
    Globals.logger.info(F("Switching to Character Generator A..."));
    Globals.cassette.setCharGenA();
    _charGen = 1;
    Globals.logger.info(F("Character Generator A selected"));
  } else {
    Globals.logger.info(F("Switching to Character Generator B..."));
    Globals.cassette.setCharGenB();
    _charGen = 2;
    Globals.logger.info(F("Character Generator B selected"));
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.info(F("Test signal deactivated after character generator toggle"));

  // Redraw the menu to show updated config value
  _drawContent();
}
