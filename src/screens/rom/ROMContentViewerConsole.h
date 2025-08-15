#ifndef ROM_CONTENT_VIEWER_CONSOLE_H
#define ROM_CONTENT_VIEWER_CONSOLE_H

#include <ConsoleScreen.h>
#include <ROM.h>

class ROMContentViewerConsole : public ConsoleScreen {
 private:
  uint16_t _currentAddress;
  static const uint16_t BYTES_PER_LINE = 16;

 public:
  ROMContentViewerConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void displayROMContent();
  uint16_t getLinesPerPage() const;
};

#endif  // ROM_CONTENT_VIEWER_CONSOLE_H
