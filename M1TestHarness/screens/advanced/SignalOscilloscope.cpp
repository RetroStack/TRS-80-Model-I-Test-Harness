#include "./SignalOscilloscope.h"

#include <Arduino.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../../globals.h"
#include "./AdvancedMenu.h"

SignalOscilloscope::SignalOscilloscope() : ContentScreen() {
  setTitleF(F("Oscilloscope"));

  // Set up button labels for paging
  const __FlashStringHelper* buttons[] = {F("M:Menu"), F("LF:Start/Stop"), F("RT:Reset"),
                                          F("UP/DN:Pg")};
  setButtonItemsF(buttons, 4);

  // Initialize state
  _plotPosition = 0;
  _lastUpdate = 0;
  _needsFullRedraw = true;
  _isRunning = true;

  // Initialize paging
  _currentPage = 0;
  _totalPages =
      1 + ((SIGNAL_COUNT + SIGNALS_PER_PAGE - 1) / SIGNALS_PER_PAGE);  // 1 overview + detail pages

  Globals.logger.infoF(F("Signal Oscilloscope initialized with %d pages"), _totalPages);
}

void SignalOscilloscope::loop() {
  unsigned long currentTime = millis();

  // Update signal states at regular intervals
  if (_isRunning && currentTime - _lastUpdate > UPDATE_INTERVAL) {
    _lastUpdate = currentTime;

    // Calculate plot dimensions from content size
    uint16_t contentWidth = _getContentWidth();
    int labelWidth = 70;  // Increased space for 4-column staggered labels
    int plotWidth = contentWidth - labelWidth - 10;  // Leave margin

    // Read state data once per update cycle for efficiency
    uint64_t stateData = Model1.getStateData();

    // Clear gap columns (increased gap - clear 2 pixels after current position)
    int gapX1 = (_plotPosition + 1) % plotWidth;
    int gapX2 = (_plotPosition + 2) % plotWidth;
    clearPlotColumn(gapX1);
    clearPlotColumn(gapX2);

    // Move to next position (skip the gap)
    _plotPosition = (_plotPosition + 1) % plotWidth;

    // Draw all signals at current position with continuous lines
    drawSignalColumn(_plotPosition, stateData);
  }

  // Handle redraw if needed
  if (_needsFullRedraw) {
    _drawContent();
  }
}

Screen* SignalOscilloscope::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_MENU) {
    Globals.logger.infoF(F("Returning to Advanced Menu from Signal Oscilloscope"));
    return new AdvancedMenu();
  }

  if (action & BUTTON_LEFT) {  // Start/Stop
    _isRunning = !_isRunning;
    Globals.logger.infoF(_isRunning ? F("Signal monitoring started")
                                    : F("Signal monitoring paused"));
  }

  if (action & BUTTON_UP) {  // Page Up
    if (_currentPage > 0) {
      _currentPage--;
      _needsFullRedraw = true;
      Globals.logger.infoF(F("Page up to %d (signals %d-%d)"), _currentPage + 1,
                           getFirstSignalOnCurrentPage(),
                           getFirstSignalOnCurrentPage() + getSignalsOnCurrentPage() - 1);
    }
  }

  if (action & BUTTON_DOWN) {  // Page Down
    if (_currentPage < _totalPages - 1) {
      _currentPage++;
      _needsFullRedraw = true;
      Globals.logger.infoF(F("Page down to %d (signals %d-%d)"), _currentPage + 1,
                           getFirstSignalOnCurrentPage(),
                           getFirstSignalOnCurrentPage() + getSignalsOnCurrentPage() - 1);
    }
  }

  if (action & BUTTON_RIGHT) {  // Reset
    // Clear the entire plot area using calculated dimensions
    uint16_t contentLeft = _getContentLeft();
    uint16_t contentTop = _getContentTop();
    uint16_t contentWidth = _getContentWidth();
    uint16_t contentHeight = _getContentHeight();
    int labelWidth = 70;
    int plotX = contentLeft + labelWidth;
    int plotY = contentTop + 10;
    int plotWidth = contentWidth - labelWidth - 10;
    int plotHeight = contentHeight - 20;

    Adafruit_GFX& gfx = M1Shield.getGFX();
    gfx.fillRect(plotX, plotY, plotWidth, plotHeight, M1Shield.convertColor(0x0000));

    _plotPosition = 0;
    _needsFullRedraw = true;
    Globals.logger.infoF(F("Signal buffer reset"));
  }

  return nullptr;
}

void SignalOscilloscope::_drawContent() {
  if (!isActive())
    return;

  Adafruit_GFX& gfx = M1Shield.getGFX();
  gfx.startWrite();

  if (_needsFullRedraw) {
    // Keep title simple
    setTitleF(F("Oscilloscope"));

    // Clear content area
    uint16_t contentLeft = _getContentLeft();
    uint16_t contentTop = _getContentTop();
    uint16_t contentWidth = _getContentWidth();
    uint16_t contentHeight = _getContentHeight();

    gfx.fillRect(contentLeft, contentTop, contentWidth, contentHeight,
                 M1Shield.convertColor(0x0000));

    // Draw static elements
    drawSignalLabels();

    _needsFullRedraw = false;
  }

  gfx.endWrite();
}

void SignalOscilloscope::clearPlotColumn(int x) {
  Adafruit_GFX& gfx = M1Shield.getGFX();
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentHeight = _getContentHeight();

  int labelWidth = 70;  // Updated to match loop()
  int plotX = contentLeft + labelWidth + x;
  int plotY = contentTop + 10;
  int plotHeight = contentHeight - 20;

  // Clear only this single vertical column (creates the gap)
  gfx.drawFastVLine(plotX, plotY, plotHeight, M1Shield.convertColor(0x0000));
}

void SignalOscilloscope::drawSignalColumn(int x, uint64_t stateData) {
  Adafruit_GFX& gfx = M1Shield.getGFX();
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentHeight = _getContentHeight();

  int labelWidth = 70;  // Updated to 70 for 4-column staggering
  int plotX = contentLeft + labelWidth + x;
  int plotY = contentTop + 10;
  int plotHeight = contentHeight - 20;

  // Draw signal values for this column
  int signalsOnPage = getSignalsOnCurrentPage();
  int firstSignal = getFirstSignalOnCurrentPage();

  int signalHeight, signalSpacing;
  if (_currentPage == 0) {
    // First page: compact view with gaps
    signalHeight = (plotHeight - 10) / signalsOnPage;
    signalSpacing = signalHeight + 1;
  } else {
    // Other pages: larger view with more space
    signalHeight = (plotHeight - 20) / signalsOnPage;
    signalSpacing = signalHeight + 2;  // Bigger gaps
  }

  for (int i = 0; i < signalsOnPage; i++) {
    int actualSignalIndex = firstSignal + i;
    bool signalValue = extractSignalFromState(stateData, actualSignalIndex);

    // Calculate position for this signal on the page
    int baseY = plotY + (i * signalSpacing);
    int yPos = baseY + (signalValue ? 1 : signalHeight - 2);

    // Draw thicker line when signal is high, thinner when low
    uint16_t color = getSignalColor(actualSignalIndex, signalValue);
    if (signalValue) {
      // For larger pages, draw thicker lines
      int thickness = (_currentPage == 0) ? 2 : 3;
      gfx.drawFastVLine(plotX, yPos, thickness, color);
    } else {
      // Draw single pixel for low signal
      gfx.drawPixel(plotX, yPos, color);
    }
  }
}

bool SignalOscilloscope::extractSignalFromState(uint64_t stateData, int signalIndex) {
  // Extract signal from packed state data - organized by pages
  // Page 1 (0-7): Address bus high A15-A08 (bits 63-56)
  // Page 2 (8-15): Address bus low A07-A00 (bits 55-48)
  // Page 3 (16-23): Data bus D7-D0 (bits 47-40)
  // Page 4 (24-31): Memory control RD,WR,IN,OUT,RAS,CAS,MUX + padding (bits 39-33)
  // Page 5 (32-35): System signals RST,IAK,INT,TST (bits 31-28)
  // Total: 36 signals

  if (signalIndex < 16) {
    // Address bus A15-A0 (bits 63-48) - Pages 1&2
    return (stateData >> (63 - signalIndex)) & 1;
  } else if (signalIndex < 24) {
    // Data bus D7-D0 (bits 47-40) - Page 3
    return (stateData >> (47 - (signalIndex - 16))) & 1;
  } else if (signalIndex < 32) {
    // Memory control + padding - Page 4
    switch (signalIndex) {
      case 24:
        return (stateData >> 39) & 1;  // RD
      case 25:
        return (stateData >> 38) & 1;  // WR
      case 26:
        return (stateData >> 37) & 1;  // IN
      case 27:
        return (stateData >> 36) & 1;  // OUT
      case 28:
        return (stateData >> 35) & 1;  // RAS
      case 29:
        return (stateData >> 34) & 1;  // CAS
      case 30:
        return (stateData >> 33) & 1;  // MUX
      case 31:
        return false;  // Padding "---"
      default:
        return false;
    }
  } else if (signalIndex < 36) {
    // System signals - Page 5
    switch (signalIndex) {
      case 32:
        return (stateData >> 31) & 1;  // RST
      case 33:
        return (stateData >> 30) & 1;  // IAK
      case 34:
        return (stateData >> 29) & 1;  // INT
      case 35:
        return (stateData >> 28) & 1;  // TST
      default:
        return false;
    }
  }
  return false;
}

void SignalOscilloscope::drawSignalLabels() {
  Adafruit_GFX& gfx = M1Shield.getGFX();
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentHeight = _getContentHeight();

  // Signal names organized for perfect page alignment - all 36 signals
  const char* signalNames[36] = {
      "A15", "A14", "A13", "A12",
      "A11", "A10", "A09", "A08",  // Page 1: Address high (bits 63-56)
      "A07", "A06", "A05", "A04",
      "A03", "A02", "A01", "A00",  // Page 2: Address low (bits 55-48)
      "D7",  "D6",  "D5",  "D4",
      "D3",  "D2",  "D1",  "D0",  // Page 3: Data bus (bits 47-40)
      "RD",  "WR",  "IN",  "OUT",
      "RAS", "CAS", "MUX", "---",  // Page 4: Memory control (bits 39-33) + padding
      "RST", "IAK", "INT", "TST"   // Page 5: System signals (bits 31-28)
  };

  gfx.setTextSize(1);
  int plotHeight = contentHeight - 20;

  int signalsOnPage = getSignalsOnCurrentPage();
  int firstSignal = getFirstSignalOnCurrentPage();

  int signalHeight, signalSpacing;
  if (_currentPage == 0) {
    // First page: compact view with gaps
    signalHeight = (plotHeight - 10) / signalsOnPage;
    signalSpacing = signalHeight + 1;
  } else {
    // Other pages: larger view with more space
    signalHeight = (plotHeight - 20) / signalsOnPage;
    signalSpacing = signalHeight + 2;  // Bigger gaps
  }

  for (int i = 0; i < signalsOnPage; i++) {
    int actualSignalIndex = firstSignal + i;

    // Debug logging for system signals page
    if (_currentPage >= 4) {
      Globals.logger.infoF(F("Page %d, i=%d, actualSignalIndex=%d, signal=%s"), _currentPage, i,
                           actualSignalIndex, signalNames[actualSignalIndex]);
    }

    // Calculate position for this signal on the page
    int baseY = contentTop + 10 + (i * signalSpacing);

    // Staggering logic based on page type
    int labelX, labelY;
    if (_currentPage == 0) {
      // First page: 3-column staggering to prevent overlap
      int column = i % 3;
      labelX = contentLeft + 2 + (column * 23);  // 23 pixels apart for 3-char labels
      labelY = baseY + (signalHeight / 2) - 3;   // Center vertically in signal space
    } else {
      // Other pages: larger text, simple left alignment with larger size
      gfx.setTextSize(2);  // Bigger text for individual pages
      labelX = contentLeft + 2;
      labelY = baseY + (signalHeight / 2) - 6;  // Center for larger text
    }

    // Use different colors for different signal groups (page-aligned)
    uint16_t textColor;
    if (actualSignalIndex < 16) {
      textColor = M1Shield.convertColor(0xFFE0);  // Yellow for address (pages 1&2)
    } else if (actualSignalIndex < 24) {
      textColor = M1Shield.convertColor(0x07E0);  // Green for data (page 3)
    } else if (actualSignalIndex < 32) {
      textColor = M1Shield.convertColor(0x07FF);  // Cyan for memory control (page 4)
    } else {
      textColor = M1Shield.convertColor(0xF81F);  // Magenta for system signals (page 5)
    }

    gfx.setTextColor(textColor);
    gfx.setCursor(labelX, labelY);

    // Don't show padding signals prominently
    if (strcmp(signalNames[actualSignalIndex], "---") == 0) {
      // Make padding signals dimmer
      gfx.setTextColor(M1Shield.convertColor(0x2104));  // Very dark gray
    }

    gfx.print(signalNames[actualSignalIndex]);

    // Reset text size for next iteration if we changed it
    if (_currentPage != 0) {
      gfx.setTextSize(1);
    }
  }
}

uint16_t SignalOscilloscope::getSignalColor(int signalIndex, bool state) {
  // Use page-aligned colors for consistency
  if (signalIndex < 16) {
    // Address bus signals (pages 1&2) - yellow
    return M1Shield.convertColor(0xFFE0);
  } else if (signalIndex < 24) {
    // Data bus signals (page 3) - green
    return M1Shield.convertColor(0x07E0);
  } else if (signalIndex < 32) {
    // Memory control signals (page 4) - cyan
    return M1Shield.convertColor(0x07FF);
  } else if (signalIndex < 36) {
    // System signals (page 5) - magenta
    return M1Shield.convertColor(0xF81F);
  }

  // Default fallback
  return M1Shield.convertColor(0xFFFF);  // White
}

int SignalOscilloscope::getSignalsOnCurrentPage() {
  if (_currentPage == 0) {
    // First page shows all signals (overview)
    return SIGNAL_COUNT;
  } else {
    // Other pages show up to SIGNALS_PER_PAGE signals
    int startSignal = (_currentPage - 1) * SIGNALS_PER_PAGE;
    int remainingSignals = SIGNAL_COUNT - startSignal;
    return (remainingSignals < SIGNALS_PER_PAGE) ? remainingSignals : SIGNALS_PER_PAGE;
  }
}

int SignalOscilloscope::getFirstSignalOnCurrentPage() {
  if (_currentPage == 0) {
    return 0;  // First page starts at signal 0 and shows all
  } else {
    // Page 1 starts at signal 0, page 2 at signal 8, page 3 at signal 16, etc.
    return (_currentPage - 1) * SIGNALS_PER_PAGE;
  }
}