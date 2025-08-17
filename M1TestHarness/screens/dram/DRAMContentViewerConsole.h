#ifndef DRAM_CONTENT_VIEWER_CONSOLE_H
#define DRAM_CONTENT_VIEWER_CONSOLE_H

#include <ConsoleScreen.h>

class DRAMContentViewerConsole : public ConsoleScreen {
 private:
  uint16_t _currentAddress;

 public:
  DRAMContentViewerConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void displayDRAMContent();
  uint16_t getLinesPerPage() const;
  uint16_t getBytesPerLine() const;
};

#endif  // DRAM_CONTENT_VIEWER_CONSOLE_H
