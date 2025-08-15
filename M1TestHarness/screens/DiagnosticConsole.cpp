#include "./DiagnosticConsole.h"

#include <Arduino.h>

#include "../globals.h"
#include "./BoardRevisionMenu.h"

DiagnosticConsole::DiagnosticConsole() : ConsoleScreen() {
  setTitleF(F("Diagnostic"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);
  setTextSize(2);

  clearButtonItems();

  Globals.logger.infoF(F("Diagnostic Screen initialized"));
}

void DiagnosticConsole::_executeOnce() {
  cls();

  setTextSize(2);
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("Nothing to do at the moment"));

  setTextColor(0x07E0, 0x0000);  // Yellow
  println(F("Press any key to move to"));
  println(F("the next screen."));
}

Screen *DiagnosticConsole::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_ANY) {
    return new BoardRevisionMenu(BoardRevisionSource::DIAGNOSTICS);
  }

  return nullptr;
}
