#include "./AboutConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "../MainMenu.h"

AboutConsole::AboutConsole() : ConsoleScreen() {
  setTitleF(F("About"));

}

void AboutConsole::_executeOnce() {
  // Display information about the test harness
  println(F("TRS-80 Model 1 Test Harness"));
  print(F("Version: "));
  println(GlobalsClass::VERSION);
  println();
  println(F("Developed by:"));
  println(F(" Ven Reddy, Marcel Erz (RetroStack)"));
  println();
  println(F("Repository: github.com/RetroStack"));
  println();

  // Display memory usage
  println();
  println(F("Memory:"));
  int freeMem = _freeMemory();
  print(F(" Free RAM: "));
  print(freeMem);
  println(F(" bytes"));

  int usedMem = 8192 - freeMem;  // Arduino Mega has 8KB SRAM
  print(F(" Used RAM: "));
  print(usedMem);
  println(F(" bytes"));

  float percentUsed = (float)usedMem / 8192.0 * 100.0;
  print(F(" RAM Usage: "));
  print(percentUsed, 1);
  println(F("%"));

  println();
  println(F("Press any key to return..."));
}

int AboutConsole::_freeMemory() {
  char top;
  extern char *__heap_start;
  extern char *__brkval;

  return &top - (__brkval ? __brkval : __heap_start);
}

Screen *AboutConsole::actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) {
  if (action & BUTTON_ANY) {
    return new MainMenu();
  }

  return nullptr;
}
