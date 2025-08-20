#ifndef ABOUT_CONSOLE_H
#define ABOUT_CONSOLE_H

#include <ConsoleScreen.h>

class AboutConsole : public ConsoleScreen {
 public:
  AboutConsole();
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY);

 protected:
  void _executeOnce() override;

 private:
  int _freeMemory();
};

#endif  // ABOUT_CONSOLE_H
