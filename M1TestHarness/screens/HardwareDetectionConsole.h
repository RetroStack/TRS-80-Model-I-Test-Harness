#ifndef HARDWARE_DETECTION_CONSOLE_H
#define HARDWARE_DETECTION_CONSOLE_H

#include <ConsoleScreen.h>

class HardwareDetectionConsole : public ConsoleScreen {
 public:
  HardwareDetectionConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void detectBoardRevision();
  void detectLowerCaseMod();
  void detectDRAMSize();
  void displayResults();
};

#endif  // HARDWARE_DETECTION_CONSOLE_H
