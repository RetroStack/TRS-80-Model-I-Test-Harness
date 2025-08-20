#ifndef WELCOME_CONSOLE_H
#define WELCOME_CONSOLE_H

#include <ConsoleScreen.h>

class WelcomeConsole : public ConsoleScreen {
 private:
 public:
  WelcomeConsole();
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY);

 private:
  void _executeOnce();
};

#endif  // WELCOME_CONSOLE_H
