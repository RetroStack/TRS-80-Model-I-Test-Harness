#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Cassette.h>
#include <Keyboard.h>
#include <M1Shield.h>
#include <Model1.h>
#include <ROM.h>
#include <SerialLogger.h>
#include <Video.h>

enum BOARD_REVISION {
  UNKNOWN = 0,

  REV_A = 1,
  REV_D = 2,
  REV_E = 3,
  REV_G = 4,
  REV_TEC_10 = 5,
  REV_TEC_20 = 6,
  REV_TEC_30 = 7,
  REV_TEC_40 = 8,
  REV_TEC_50 = 9,
};

class GlobalsClass {
 private:
  BOARD_REVISION boardRevision;
  bool hasLowerCaseMod;
  uint16_t dramSizeKB;

 public:
  Cassette cassette;
  Keyboard keyboard;
  ROM rom;
  SerialLogger logger;
  Video video;

  static const char VERSION[];

  GlobalsClass();

  BOARD_REVISION getBoardRevision() const;
  void setBoardRevision(BOARD_REVISION rev);
  const __FlashStringHelper* getBoardRevisionString(BOARD_REVISION rev) const;

  bool getHasLowerCaseMod() const;
  void setHasLowerCaseMod(bool hasLowerCase);

  uint16_t getDRAMSizeKB() const;
  void setDRAMSizeKB(uint16_t sizeKB);
};

extern GlobalsClass Globals;

#endif  // GLOBALS_H
