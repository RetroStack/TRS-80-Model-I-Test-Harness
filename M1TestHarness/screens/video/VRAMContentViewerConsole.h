#ifndef VRAM_CONTENT_VIEWER_CONSOLE_H
#define VRAM_CONTENT_VIEWER_CONSOLE_H

#include <ConsoleScreen.h>

class VRAMContentViewerConsole : public ConsoleScreen {
 private:
  uint16_t _currentAddress;

 public:
  VRAMContentViewerConsole();
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void displayVRAMContent();
  uint16_t getLinesPerPage() const;
  uint16_t getBytesPerLine() const;
};

#endif  // VRAM_CONTENT_VIEWER_CONSOLE_H
