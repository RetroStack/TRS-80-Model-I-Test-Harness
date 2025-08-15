#ifndef DIAGNOSTIC_CONSOLE_H
#define DIAGNOSTIC_CONSOLE_H

#include <ConsoleScreen.h>

class DiagnosticConsole : public ConsoleScreen {
 private:
 public:
  DiagnosticConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 private:
  void _executeOnce();
};

#endif  // DIAGNOSTIC_CONSOLE_H
