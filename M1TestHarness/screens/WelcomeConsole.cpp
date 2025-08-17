#include "./WelcomeConsole.h"

#include <Arduino.h>

#include "../globals.h"
#include "./DiagnosticConsole.h"

WelcomeConsole::WelcomeConsole() : ConsoleScreen() {
  setTitleF(F("Test Harness"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);
  setTextSize(2);

  clearButtonItems();
  
  // Enable auto-forward after 5 seconds
  setAutoForward(true, 5000);

  Globals.logger.infoF(F("Welcome Screen initialized"));
}

void WelcomeConsole::_executeOnce() {
  cls();
  setTextColor(0x07E0, 0x0000);  // Green
  setTextSize(2);
  println(F("TRS-80 MODEL 1"));
  println();

  setTextSize(1);
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("This test harness verifies individual"));
  println(F("features of the TRS-80 Model 1."));
  println();

  println(F("The Arduino must be attached to"));
  println(F("the Model 1 through the edge connector"));
  println();

  setTextColor(0x07FF, 0x0000);  // White
  println(F("More info available at:"));
  setTextColor(0xFFE0, 0x0000);  // Yellow
  println(F("www.github.com/RetroStack/"));
  println(F("TRS-80-Model-I-Test-Harness"));
  println();

  setTextColor(0xFFFF, 0x0000);  // White
  println(F("In the next screen, basic"));
  println(F("functionality will be tested to"));
  println(F("ensure communication with the"));
  println(F("Model 1 is possible."));
  println();

  println(F("Should there be an issue, a"));
  println(F("revision selector will help"));
  println(F("identify potential offending"));
  println(F("components or traces."));
  println();

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("Press any key to start the"));
  println(F("high-level diagnostics."));
  println();
  
  setTextColor(0x07FF, 0x0000);  // Cyan
  println(F("(Auto-forward in 5 seconds)"));
}

Screen *WelcomeConsole::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & (BUTTON_ANY | BUTTON_MENU)) {
    return new DiagnosticConsole();
  }

  return nullptr;
}


