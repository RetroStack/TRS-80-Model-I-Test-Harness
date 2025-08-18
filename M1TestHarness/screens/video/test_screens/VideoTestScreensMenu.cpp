#include "./VideoTestScreensMenu.h"

#include <Arduino.h>
#include <Model1.h>

#include "../../../globals.h"
#include "../VideoMenu.h"

VideoTestScreensMenu::VideoTestScreensMenu() : MenuScreen() {
  setTitleF(F("Test Screens"));

  _currentTestScreen = -1;  // Non selected

  const __FlashStringHelper *menuItems[] = {F("All Characters"), F("Frame"), F("White"),
                                            F("Black"),          F("All @"), F("All 1st Char")};
  setMenuItemsF(menuItems, 6);

}

Screen *VideoTestScreensMenu::_getSelectedMenuItemScreen(int index) {
  if (index == -1) {
    return new VideoMenu();
  }

  // Briefly activate test signal for video operations
  Model1.activateTestSignal();
  Globals.logger.infoF(F("Test signal activated for character mode toggle"));

  uint16_t videoAddress;
  switch (index) {
    case 0: {  // All Characters

      videoAddress = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(videoAddress + i, i & 0xFF);  // Fill VRAM with test pattern
      }
      break;
    }

    case 1: {  // Frame

      Globals.video.cls();
      for (uint16_t i = 0; i < 64; i++) {
        Model1.writeMemory(Globals.video.getAddress(i, 0), 0xFF);
        Model1.writeMemory(Globals.video.getAddress(i, 15), 0xFF);
      }
      for (uint16_t i = 1; i < 15; i++) {
        Model1.writeMemory(Globals.video.getAddress(0, i), 0xFF);
        Model1.writeMemory(Globals.video.getAddress(63, i), 0xFF);
      }
      break;
    }

    case 2: {  // White
      uint16_t address = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(address + i, 0xFF);
      }
      break;
    }

    case 3: {  // Black
      uint16_t address = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(address + i, 0x20);
      }
      break;
    }

    case 4: {  // All @
      uint16_t address = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(address + i, 0x40);
      }
      break;
    }

    case 5: {  // All 1st Char
      uint16_t address = Globals.video.getAddress(0, 0);
      for (uint16_t i = 0; i < 1024; i++) {
        Model1.writeMemory(address + i, 0x00);
      }
      break;
    }
  }

  // Deactivate test signal after operation
  Model1.deactivateTestSignal();
  Globals.logger.infoF(F("Test signal deactivated after character mode toggle"));

  // Remember the test screen selected to highlight it
  _currentTestScreen = index;

  // Redraw the menu to show updated config value
  _drawContent();

  return nullptr;
}

const __FlashStringHelper *VideoTestScreensMenu::_getMenuItemConfigValueF(uint8_t index) {
  if (index == _currentTestScreen) {
    return F("*");
  }
  return nullptr;  // No config value for other indices
}
