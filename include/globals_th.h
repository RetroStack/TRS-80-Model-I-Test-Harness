#ifndef GLOBALS_TH_H
#define GLOBALS_TH_H

#include <Arduino.h>
#include <Cassette.h>
#include <Keyboard.h>
#include <Model1.h>
#include <ROM.h>
#include <SerialLogger.h>
#include <Video.h>

#include "lcd_display.h"
#include "menu_th.h"
#include "model1_th.h"
#include "utils_th.h"

// Model1 is a global instance from the M1 TRS-80 Arduino library
extern Cassette cassette;
extern Keyboard keyboard;
extern ROM rom;
extern SerialLogger logger;
extern Video video;
extern const char VERSION[];  // version of the test harness

// Global utility functions
bool isCommand(const char* input, const char* command);

class th {
 public:
  // static Model1* model1;
  static size_t g_ramSize;      // RAM size in bytes
  static bool g_echoToDisplay;  // Flag to echo output to display
  static SerialLogger logger;
};

#endif  // GLOBALS_TH_H
