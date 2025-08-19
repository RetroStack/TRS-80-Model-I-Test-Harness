#ifndef ROM_CONTENT_VIEWER_CONSOLE_H
#define ROM_CONTENT_VIEWER_CONSOLE_H

#include <ConsoleScreen.h>
#include <ROM.h>

class ROMContentViewerConsole : public ConsoleScreen {
 private:
  uint16_t _currentAddress;

 public:
  ROMContentViewerConsole();
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void displayROMContent();
  uint16_t getLinesPerPage() const;
  uint16_t getBytesPerLine() const;
};

#endif  // ROM_CONTENT_VIEWER_CONSOLE_H
