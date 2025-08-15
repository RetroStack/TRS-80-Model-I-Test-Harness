#include "./VideoTestScreensMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../../globals.h"
#include "../VideoMenu.h"

VideoTestScreensMenu::VideoTestScreensMenu() : MenuScreen() {
  setTitle((const char *)F("Test Screens"));

  _currentTestScreen = -1;  // Non selected

  const char *menuItems[] = {(const char *)F("All Characters"), (const char *)F("Frame"),
                             (const char *)F("White"),          (const char *)F("Black"),
                             (const char *)F("All @"),          (const char *)F("All 1st Char")};
  setMenuItems(menuItems, 6);

  Globals.logger.info(F("Test Screens Menu initialized"));
}

Screen *VideoTestScreensMenu::_getSelectedMenuItemScreen(int index) {
  if (index == -1) {
    Globals.logger.info(F("Returning to Video Menu from Test Screens Menu"));
    return new VideoMenu();
  }

  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.info(F("Test signal activated for character mode toggle"));

  uint16_t videoAddress;
  switch (index) {
    case 0: {  // All Characters
      Globals.logger.info(F("Opening All Characters Test Screen"));

      videoAddress = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(videoAddress + i, i & 0xFF);  // Fill VRAM with test pattern
      }
      break;
    }

    case 1: {  // Frame
      Globals.logger.info(F("Opening Frame Test Screen"));

      Globals.video.cls();
      for (uint16_t i = 0; i < 64; i++) {
        Model1.writeMemory(Globals.video.getAddress(i, 0) + i, 0xFF);
        Model1.writeMemory(Globals.video.getAddress(i, 15), 0xFF);
      }
      for (uint16_t i = 1; i < 15; i++) {
        Model1.writeMemory(Globals.video.getAddress(0, i), 0xFF);
        Model1.writeMemory(Globals.video.getAddress(63, i), 0xFF);
      }
      break;
    }

    case 2: {  // White
      Globals.logger.info(F("Opening White Test Screen"));
      Globals.video.cls((char)0xFF);
      break;
    }

    case 3: {  // Black
      Globals.logger.info(F("Opening Black Test Screen"));
      Globals.video.cls();
      break;
    }

    case 4: {  // All @
      Globals.logger.info(F("Opening All @ Test Screen"));
      Globals.video.cls((char)0x40);
      break;
    }

    case 5: {  // All 1st Char
      Globals.logger.info(F("Opening All 1st Char Test Screen"));
      Globals.video.cls((char)0x00);
      break;
    }
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.info(F("Test signal deactivated after character mode toggle"));

  // Remember the test screen selected to highlight it
  _currentTestScreen = index;

  // Redraw the menu to show updated config value
  _drawContent();

  return nullptr;
}

const char *VideoTestScreensMenu::_getMenuItemConfigValue(uint8_t index) {
  if (index == _currentTestScreen) {
    return (const char *)F("*");
  }
  return nullptr;  // No config value for other indices
}
