#include "sys_config_th.h"

#include "globals_th.h"
#include "menu_th.h"
#include "utils_th.h"

namespace SysConfigTH {

void showMenu() {
  println(F("s) System configuration"));
  println(F("r) ROM information"));
  println(F("h) Hardware status"));
  println(F("d) Debug settings"));
  println(F("x) Back to main menu"));
  println(F("B) Bus pin statuses"));
  println(F("C) Clear screen"));
  printTestSignalState();
  println(F("X) Reset Test Harness"));
  println(F("?  help"));
}

void handleInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case 's':
      println(F("[SYSTEM] Configuration - TODO"));
      break;
    case 'r':
      println(F("[ROM] Information - TODO"));
      break;
    case 'h':
      println(F("[HARDWARE] Status - TODO"));
      break;
    case 'd':
      println(F("[DEBUG] Settings - TODO"));
      break;
    case 'x':
      Menu::pop();
      break;
    case '?': /* Menu system auto-shows */
      break;
    default:
      println(F("Invalid input"));
      break;
  }
}

}  // namespace SysConfigTH
