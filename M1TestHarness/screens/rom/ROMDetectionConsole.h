#ifndef ROM_DETECTION_CONSOLE_H
#define ROM_DETECTION_CONSOLE_H

#include <ConsoleScreen.h>
#include <ROM.h>

class ROMDetectionConsole : public ConsoleScreen {
 public:
  ROMDetectionConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 private:
  void _executeOnce();
};

#endif  // ROM_DETECTION_CONSOLE_H
