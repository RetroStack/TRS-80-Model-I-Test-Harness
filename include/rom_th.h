#ifndef ROM_TH_H
#define ROM_TH_H

#include <Arduino.h>
#include "utils_th.h"
#include "globals_th.h"
#include "menu_th.h"

namespace RomTH {
  /* ---------- public API used by the menu engine ---------- */
  void showMenu();            // print submenu (called on entry)
  void handleInput(const char* input);   // dispatch input (memory-safe version)

  /* ---------- test routines / helpers (stubs today) ------- */
  void bytesRead();            // b
  void checksumsForROMs();    // c
  void clearScreen();         // C
  void dumpROM();             // d
  void version();             // v
  void getROMStartAddress();  // a
  void getROMLength();        // l
}
#endif // ROM_TH_H

