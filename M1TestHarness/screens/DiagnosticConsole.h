#ifndef DIAGNOSTIC_CONSOLE_H
#define DIAGNOSTIC_CONSOLE_H

#include <ConsoleScreen.h>

// Forward declaration of result structures
struct ResetButtonTestResult;

class DiagnosticConsole : public ConsoleScreen {
 private:
 public:
  DiagnosticConsole();
  bool open() override;
  void close() override;
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 private:
  void _executeOnce();
  ResetButtonTestResult verifyResetButton();
};

#endif  // DIAGNOSTIC_CONSOLE_H
