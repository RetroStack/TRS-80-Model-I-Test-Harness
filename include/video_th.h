#ifndef VIDEO_TH_H
#define VIDEO_TH_H

#include <Arduino.h>
#include "utils_th.h"

namespace VideoTH {

  void showMenu();
  void handleInput(const char* input);
  void showTests();
  void handleTestsInput(const char* input);
  void toggleCharacterMode();
}

#endif // VIDEO_TH_H
