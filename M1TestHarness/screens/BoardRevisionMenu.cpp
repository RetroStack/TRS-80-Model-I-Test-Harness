#include "./BoardRevisionMenu.h"

#include <Arduino.h>

#include "../globals.h"
#include "./DiagnosticConsole.h"
#include "./MainMenu.h"

BoardRevisionMenu::BoardRevisionMenu(BoardRevisionSource source) : MenuScreen(), _source(source) {
  setTitle((const char *)F("Board Revision"));

  // Create menu items using the global board revision strings
  const char *menuItems[10];
  menuItems[0] = Globals.getBoardRevisionString(UNKNOWN);
  menuItems[1] = Globals.getBoardRevisionString(REV_A);
  menuItems[2] = Globals.getBoardRevisionString(REV_D);
  menuItems[3] = Globals.getBoardRevisionString(REV_E);
  menuItems[4] = Globals.getBoardRevisionString(REV_G);
  menuItems[5] = Globals.getBoardRevisionString(REV_TEC_10);
  menuItems[6] = Globals.getBoardRevisionString(REV_TEC_20);
  menuItems[7] = Globals.getBoardRevisionString(REV_TEC_30);
  menuItems[8] = Globals.getBoardRevisionString(REV_TEC_40);
  menuItems[9] = Globals.getBoardRevisionString(REV_TEC_50);
  setMenuItems(menuItems, 10);

  Globals.logger.info(F("Board Revision screen initialized"));
}

Screen *BoardRevisionMenu::_getSelectedMenuItemScreen(int index) {
  Screen *returnScreen = nullptr;

  // Determine the return screen based on source
  if (_source == BoardRevisionSource::DIAGNOSTICS) {
    returnScreen = new DiagnosticConsole();
  } else {
    returnScreen = new MainMenu();
  }

  switch (index) {
    case 0:  // Unknown
      Globals.logger.info(F("Setting board revision to Unknown"));
      Globals.setBoardRevision(UNKNOWN);
      return returnScreen;

    case 1:  // Revision A
      Globals.logger.info(F("Setting board revision to A"));
      Globals.setBoardRevision(REV_A);
      return returnScreen;

    case 2:  // Revision D
      Globals.logger.info(F("Setting board revision to D"));
      Globals.setBoardRevision(REV_D);
      return returnScreen;

    case 3:  // Revision E
      Globals.logger.info(F("Setting board revision to E"));
      Globals.setBoardRevision(REV_E);
      return returnScreen;

    case 4:  // Revision G
      Globals.logger.info(F("Setting board revision to G"));
      Globals.setBoardRevision(REV_G);
      return returnScreen;

    case 5:  // TEC (Jap) 10
      Globals.logger.info(F("Setting board revision to TEC 10"));
      Globals.setBoardRevision(REV_TEC_10);
      return returnScreen;

    case 6:  // TEC (Jap) 20
      Globals.logger.info(F("Setting board revision to TEC 20"));
      Globals.setBoardRevision(REV_TEC_20);
      return returnScreen;

    case 7:  // TEC (Jap) 30
      Globals.logger.info(F("Setting board revision to TEC 30"));
      Globals.setBoardRevision(REV_TEC_30);
      return returnScreen;

    case 8:  // TEC (Jap) 40
      Globals.logger.info(F("Setting board revision to TEC 40"));
      Globals.setBoardRevision(REV_TEC_40);
      return returnScreen;

    case 9:  // TEC (Jap) 50
      Globals.logger.info(F("Setting board revision to TEC 50"));
      Globals.setBoardRevision(REV_TEC_50);
      return returnScreen;

    case -1:  // Back (handled by menu system)
      return returnScreen;

    default:
      delete returnScreen;  // Clean up if we're not using it
      return nullptr;
  }
}

const char *BoardRevisionMenu::_getMenuItemConfigValue(uint8_t index) {
  // Show current board revision selection indicator
  BOARD_REVISION currentRevision = Globals.getBoardRevision();

  switch (index) {
    case 0:  // Unknown
      return (currentRevision == UNKNOWN) ? (const char *)F("<") : nullptr;

    case 1:  // Revision A
      return (currentRevision == REV_A) ? (const char *)F("<") : nullptr;

    case 2:  // Revision D
      return (currentRevision == REV_D) ? (const char *)F("<") : nullptr;

    case 3:  // Revision E
      return (currentRevision == REV_E) ? (const char *)F("<") : nullptr;

    case 4:  // Revision G
      return (currentRevision == REV_G) ? (const char *)F("<") : nullptr;

    case 5:  // TEC (Jap) 10
      return (currentRevision == REV_TEC_10) ? (const char *)F("<") : nullptr;

    case 6:  // TEC (Jap) 20
      return (currentRevision == REV_TEC_20) ? (const char *)F("<") : nullptr;

    case 7:  // TEC (Jap) 30
      return (currentRevision == REV_TEC_30) ? (const char *)F("<") : nullptr;

    case 8:  // TEC (Jap) 40
      return (currentRevision == REV_TEC_40) ? (const char *)F("<") : nullptr;

    case 9:  // TEC (Jap) 50
      return (currentRevision == REV_TEC_50) ? (const char *)F("<") : nullptr;

    default:
      return nullptr;
  }
}
