#ifndef ABOUT_CONSOLE_H
#define ABOUT_CONSOLE_H

#include <ConsoleScreen.h>

class AboutConsole : public ConsoleScreen {
 public:
  AboutConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  int _freeMemory();
};

#endif  // ABOUT_CONSOLE_H
