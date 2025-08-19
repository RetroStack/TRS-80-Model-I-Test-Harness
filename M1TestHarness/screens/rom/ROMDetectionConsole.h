#ifndef ROM_DETECTION_CONSOLE_H
#define ROM_DETECTION_CONSOLE_H

#include <ConsoleScreen.h>
#include <ROM.h>

class ROMDetectionConsole : public ConsoleScreen {
 public:
  ROMDetectionConsole();
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) override;

 private:
  void _executeOnce();
};

#endif  // ROM_DETECTION_CONSOLE_H
