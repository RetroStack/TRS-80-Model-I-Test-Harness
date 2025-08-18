#include "./globals.h"

// Store version string in PROGMEM to save RAM
const char GlobalsClass::VERSION[] PROGMEM = "1.0.0";

GlobalsClass Globals;

GlobalsClass::GlobalsClass() {
  boardRevision = UNKNOWN;
  hasLowerCaseMod = false;
  dramSizeKB = 0;
  detectedDRAMSizeKB = 0;  // Initialize detected DRAM size

  Model1.setLogger(logger);
  M1Shield.setLogger(logger);

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

const __FlashStringHelper* GlobalsClass::getBoardRevisionString(BOARD_REVISION rev) const {
  switch (rev) {
    case REV_A:
      return F("Rev A");
    case REV_D:
      return F("Rev D");
    case REV_E:
      return F("Rev E");
    case REV_G:
      return F("Rev G");
    case REV_TEC_10:
      return F("TEC 10");
    case REV_TEC_20:
      return F("TEC 20");
    case REV_TEC_30:
      return F("TEC 30");
    case REV_TEC_40:
      return F("TEC 40");
    case REV_TEC_50:
      return F("TEC 50");
    case UNKNOWN:
    default:
      return F("Unknown");
  }
}

bool GlobalsClass::getHasLowerCaseMod() const {
  return hasLowerCaseMod;
}

void GlobalsClass::setHasLowerCaseMod(bool hasLowerCase) {
  hasLowerCaseMod = hasLowerCase;
}

uint16_t GlobalsClass::getDRAMSizeKB() const {
  return dramSizeKB;
}

void GlobalsClass::setDRAMSizeKB(uint16_t sizeKB) {
  dramSizeKB = sizeKB;
}

uint16_t GlobalsClass::getDetectedDRAMSizeKB() const {
  return detectedDRAMSizeKB;
}

void GlobalsClass::setDetectedDRAMSizeKB(uint16_t sizeKB) {
  detectedDRAMSizeKB = sizeKB;
  if (dramSizeKB == 0) {
    dramSizeKB = sizeKB;  // Set current size if not already set
  }
}