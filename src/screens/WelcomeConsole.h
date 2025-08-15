#ifndef WELCOME_CONSOLE_H
#define WELCOME_CONSOLE_H

#include <ConsoleScreen.h>

class WelcomeConsole : public ConsoleScreen {
 private:
 public:
  WelcomeConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 private:
  void _executeOnce();
};

#endif  // WELCOME_CONSOLE_H
