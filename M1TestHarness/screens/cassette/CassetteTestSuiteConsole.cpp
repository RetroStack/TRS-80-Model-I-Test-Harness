#include "./CassetteTestSuiteConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./CassetteMenu.h"

CassetteTestSuiteConsole::CassetteTestSuiteConsole() : ConsoleScreen() {
  setTitle((const char *)F("Cassette Test Suite"));

  Globals.logger.info(F("Cassette Test Suite screen initialized"));
}

void CassetteTestSuiteConsole::_executeOnce() {
  // TODO
  // Use the cassette interface to check if remote is connected, in and out works as expected.

  println();
  println(F("Press any key to return..."));
}

Screen *CassetteTestSuiteConsole::actionTaken(ActionTaken action, uint8_t offsetX,
                                              uint8_t offsetY) {
  if (action & BUTTON_ANY) {
    return new CassetteMenu();
  }

  return nullptr;
}
