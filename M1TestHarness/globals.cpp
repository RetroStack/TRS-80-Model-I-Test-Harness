#include "./globals.h"

// Store version string in PROGMEM to save RAM
const char GlobalsClass::VERSION[] PROGMEM = "1.0.0";

GlobalsClass Globals;

GlobalsClass::GlobalsClass() {
  boardRevision = UNKNOWN;

  Model1.setLogger(logger);

  cassette.setLogger(logger);
  keyboard.setLogger(logger);
  rom.setLogger(logger);
  video.setLogger(logger);
}

BOARD_REVISION GlobalsClass::getBoardRevision() const {
  return boardRevision;
}

void GlobalsClass::setBoardRevision(BOARD_REVISION rev) {
  boardRevision = rev;
}

const char* GlobalsClass::getBoardRevisionString(BOARD_REVISION rev) const {
  switch (rev) {
    case REV_A:
      return (const char*)F("Rev A");
    case REV_D:
      return (const char*)F("Rev D");
    case REV_E:
      return (const char*)F("Rev E");
    case REV_G:
      return (const char*)F("Rev G");
    case REV_TEC_10:
      return (const char*)F("TEC 10");
    case REV_TEC_20:
      return (const char*)F("TEC 20");
    case REV_TEC_30:
      return (const char*)F("TEC 30");
    case REV_TEC_40:
      return (const char*)F("TEC 40");
    case REV_TEC_50:
      return (const char*)F("TEC 50");
    case UNKNOWN:
    default:
      return (const char*)F("Unknown");
  }
}