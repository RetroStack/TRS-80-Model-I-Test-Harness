#include "./VRAMContentViewerConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./VideoMenu.h"

VRAMContentViewerConsole::VRAMContentViewerConsole() : ConsoleScreen() {
  setTitleF(F("VRAM Viewer"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  _currentAddress = 0x3C00;  // VRAM starts at 0x3C00

  // Set button labels for navigation
  const __FlashStringHelper *buttons[] = {F("M:Exit"), F("UP:Prev"), F("DN:Next")};
  setButtonItemsF(buttons, 3);

}

void VRAMContentViewerConsole::_executeOnce() {
  displayVRAMContent();
}

void VRAMContentViewerConsole::displayVRAMContent() {
  cls();
  setTextColor(0xFFFF, 0x0000);  // White

  // Get dynamic dimensions based on available screen space
  uint16_t linesPerPage = getLinesPerPage();
  uint16_t bytesPerLine = getBytesPerLine();

  // Display hex dump
  Model1.activateTestSignal();
  for (uint16_t line = 0; line < linesPerPage; line++) {
    uint16_t lineAddress = _currentAddress + (line * bytesPerLine);

    // Build complete line as a single string
    String completeLine = "";

    // Address column (magenta will be set before printing)
    String addressPart = "";
    if (lineAddress < 0x1000)
      addressPart += "0";
    if (lineAddress < 0x0100)
      addressPart += "0";
    if (lineAddress < 0x0010)
      addressPart += "0";
    addressPart += String(lineAddress, HEX);
    addressPart += ": ";

    // Hex bytes
    String hexPart = "";
    for (uint16_t byte = 0; byte < bytesPerLine; byte++) {
      uint16_t address = lineAddress + byte;
      if (address >= 0x3C00 && address <= 0x3FFF)  // VRAM range (1KB)
      {
        uint8_t value = Model1.readMemory(address);
        if (value < 0x10)
          hexPart += "0";
        hexPart += String(value, HEX);
        hexPart += " ";
      } else {
        hexPart += "-- ";
      }
    }

    // ASCII/Character representation
    String charPart = " ";
    for (uint16_t byte = 0; byte < bytesPerLine; byte++) {
      uint16_t address = lineAddress + byte;
      if (address >= 0x3C00 && address <= 0x3FFF) {
        uint8_t value = Model1.readMemory(address);
        // Standard hex editor convention:
        // 0x00-0x1F: Control characters -> "."
        // 0x20-0x7F: Printable ASCII -> actual character
        // 0x80-0xFF: Extended/graphics characters -> "."
        if (value >= 0x20 && value <= 0x7F) {
          charPart += (char)value;
        } else {
          charPart += ".";
        }
      } else {
        charPart += " ";
      }
    }

    // Print address in yellow
    setTextColor(0xFFE0, 0x0000);  // Yellow
    print(addressPart);

    // Print hex bytes in cyan
    setTextColor(0x07FF, 0x0000);  // Cyan
    print(hexPart);

    // Print characters in white
    setTextColor(0xFFFF, 0x0000);  // White
    println(charPart);
  }
  Model1.deactivateTestSignal();
}

Screen *VRAMContentViewerConsole::actionTaken(ActionTaken action, uint8_t offsetX,
                                              uint8_t offsetY) {
  if (action & BUTTON_MENU) {
    return new VideoMenu();
  }

  if (action & UP_ANY) {
    uint16_t linesPerPage = getLinesPerPage();
    uint16_t bytesPerLine = getBytesPerLine();
    uint16_t pageSize = bytesPerLine * linesPerPage;
    // Ensure we don't go below VRAM start (0x3C00) and have a full page to go back
    if (_currentAddress >= 0x3C00 + pageSize) {
      _currentAddress -= pageSize;
      displayVRAMContent();
    }
    return nullptr;
  }

  if (action & DOWN_ANY) {
    uint16_t linesPerPage = getLinesPerPage();
    uint16_t bytesPerLine = getBytesPerLine();
    uint16_t pageSize = bytesPerLine * linesPerPage;
    // Ensure the next page fits entirely within VRAM (0x3C00-0x3FFF, 1KB)
    if (_currentAddress + pageSize <= 0x4000) {
      _currentAddress += pageSize;
      displayVRAMContent();
    }
    return nullptr;
  }

  return nullptr;
}

uint16_t VRAMContentViewerConsole::getLinesPerPage() const {
  // Account for header (2 lines: title + blank line)
  // Use the ConsoleScreen's internal dimensions and line height
  uint16_t availableHeight = _getContentHeight();
  uint16_t headerHeight = 2 * 8;  // Assuming default text size (8 pixels per line)

  // Use current text size to calculate line height
  uint16_t lineHeight = 8;  // Default size 1 text height

  // Calculate usable height after header
  uint16_t usableHeight = availableHeight - headerHeight;

  // Calculate how many lines fit, with a minimum of 5 lines
  uint16_t calculatedLines = usableHeight / lineHeight;

  // Ensure we have at least 5 lines and don't exceed reasonable limits
  if (calculatedLines < 5)
    calculatedLines = 5;

  return calculatedLines;
}

uint16_t VRAMContentViewerConsole::getBytesPerLine() const {
  // Get available content width
  uint16_t contentWidth = _getContentWidth();

  // Calculate character width for default text size
  uint16_t charWidth = 6;  // Default size 1 character width
  uint16_t maxChars = contentWidth / charWidth;

  // Format: "XXXX: " (6 chars) + hex bytes (3 chars each) + " " (1 char) + chars (1 char each)
  // Total per byte: 4 characters (3 hex + 1 char)
  // Fixed overhead: 7 characters (address + ": " + space between hex and chars)

  // Calculate: maxChars = 7 + (4 * bytesPerLine)
  // Solve for bytesPerLine: (maxChars - 7) / 4
  uint16_t calculatedBytes = (maxChars > 7) ? (maxChars - 7) / 4 : 8;

  // Ensure reasonable bounds: minimum 8, maximum 32
  if (calculatedBytes < 8)
    calculatedBytes = 8;
  else if (calculatedBytes > 32)
    calculatedBytes = 32;

  // Prefer multiples of 8 for better alignment
  calculatedBytes = (calculatedBytes / 8) * 8;
  if (calculatedBytes == 0)
    calculatedBytes = 8;

  return calculatedBytes;
}
