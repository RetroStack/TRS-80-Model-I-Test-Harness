#include "./SignalOscilloscope.h"

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Model1.h>

#include "../../globals.h"
#include "./AdvancedMenu.h"
#include "./AdvancedSignalController.h"

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
  // Let the global signal controller handle signal updates
  AdvancedSignals.loop();

  unsigned long currentTime = millis();

  // Update signal states at regular intervals
  if (_isRunning && currentTime - _lastUpdate > UPDATE_INTERVAL) {
    _lastUpdate = currentTime;

    // Calculate plot dimensions from content size
    uint16_t contentWidth = _getContentWidth();
    int labelWidth = 70;                            // Increased space for 4-column staggered labels
    int plotWidth = contentWidth - labelWidth - 2;  // Leave margin

    // Read state data once per update cycle for efficiency
    uint64_t stateData = Model1.getStateData();

    // Clear gap columns (increased gap - clear 2 pixels after current position)
    int gapX1 = (_plotPosition + 1) % plotWidth;
    int gapX2 = (_plotPosition + 2) % plotWidth;
    int gapX3 = (_plotPosition + 3) % plotWidth;
    clearPlotColumn(gapX1, false);
    clearPlotColumn(gapX2, false);
    clearPlotColumn(gapX3, true);

    // Move to next position (skip the gap)
    _plotPosition = (_plotPosition + 1) % plotWidth;
    clearPlotColumn(gapX1, true);

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
    int plotY = contentTop + 2;
    int plotWidth = contentWidth - labelWidth - 2;
    int plotHeight = contentHeight - 2;

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

void SignalOscilloscope::clearPlotColumn(int x, bool clearGap) {
  Adafruit_GFX& gfx = M1Shield.getGFX();
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentHeight = _getContentHeight();

  int labelWidth = 70;  // Updated to match loop()
  int plotX = contentLeft + labelWidth + x;
  int plotY = contentTop + 2;
  int plotHeight = contentHeight - 2;

  // Clear only this single vertical column (creates the gap)
  if (clearGap) {
    gfx.drawFastVLine(plotX, plotY, plotHeight, M1Shield.convertColor(0x0000));
  } else {
    gfx.drawFastVLine(plotX, plotY, plotHeight, M1Shield.convertColor(0xF800));
  }
}

void SignalOscilloscope::drawSignalColumn(int x, uint64_t stateData) {
  Adafruit_GFX& gfx = M1Shield.getGFX();
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentHeight = _getContentHeight();

  int labelWidth = 70;  // Updated to 70 for 4-column staggering
  int plotX = contentLeft + labelWidth + x;
  int plotY = contentTop + 2;
  int plotHeight = contentHeight - 2;

  // Draw signal values for this column
  int signalsOnPage = getSignalsOnCurrentPage();
  int firstSignal = getFirstSignalOnCurrentPage();

  int signalHeight, signalSpacing;
  if (_currentPage == 0) {
    // First page: compact view, fit all signals exactly
    signalHeight = plotHeight / signalsOnPage;
    signalSpacing = signalHeight;  // No extra gap, fits perfectly
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
  // Extract signal from packed state data - matches getStateData() layout:
  // Bits 63-48: Address bus A15-A0 (16 bits)
  // Bits 47-40: Data bus D7-D0 (8 bits)
  // Bits 39-32: Memory control RD,WR,IN,OUT,RAS,CAS,MUX,reserved (8 bits)
  // Bits 31-24: System signals SYS_RES,INT_ACK,INT,TEST,WAIT,reserved (8 bits)
  // Bits 23-0:  Reserved for future expansion
  // Total displayed: 37 signals (Address + Data + Memory + padding + System)

  if (signalIndex < 16) {
    // Address bus A15-A0 (bits 63-48) - indices 0-15
    return (stateData >> (63 - signalIndex)) & 1;
  } else if (signalIndex < 24) {
    // Data bus D7-D0 (bits 47-40) - indices 16-23
    return (stateData >> (47 - (signalIndex - 16))) & 1;
  } else if (signalIndex < 31) {
    // Memory control signals - indices 24-30 (RD, WR, IN, OUT, RAS, CAS, MUX)
    return (stateData >> (39 - (signalIndex - 24))) & 1;
  } else if (signalIndex == 31) {
    // Padding/unused - index 31
    return false;
  } else if (signalIndex < 37) {
    // System signals - indices 32-36 (RST, IACK, INT, TEST, WAIT)
    switch (signalIndex) {
      case 32:
        return (stateData >> 31) & 1;  // RST (SYS_RES)
      case 33:
        return (stateData >> 30) & 1;  // IACK (INT_ACK)
      case 34:
        return (stateData >> 29) & 1;  // INT
      case 35:
        return (stateData >> 28) & 1;  // TEST
      case 36:
        return (stateData >> 27) & 1;  // WAIT
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

  // Use the class member signal names array
  const char* const* signalNames = _signalNames;

  gfx.setTextSize(1);
  int plotHeight = contentHeight - 2;

  int signalsOnPage = getSignalsOnCurrentPage();
  int firstSignal = getFirstSignalOnCurrentPage();

  int signalHeight, signalSpacing;
  if (_currentPage == 0) {
    // First page: compact view, fit all signals exactly
    signalHeight = plotHeight / signalsOnPage;
    signalSpacing = signalHeight;  // No extra gap, fits perfectly
  } else {
    // Other pages: larger view with more space
    signalHeight = (plotHeight - 20) / signalsOnPage;
    signalSpacing = signalHeight + 2;  // Bigger gaps
  }

  for (int i = 0; i < signalsOnPage; i++) {
    int actualSignalIndex = firstSignal + i;

    // Calculate position for this signal on the page
    int baseY = contentTop + 2 + (i * signalSpacing);

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
      labelX = contentLeft + 15;
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
  // Special case: padding signal '---' (index 31)
  if (signalIndex == 31) {
    return M1Shield.convertColor(0x2104);  // Very dark gray
  }
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
  } else if (signalIndex <= 36) {
    // System signals (page 5) including WAIT - magenta
    return M1Shield.convertColor(0xF81F);
  }

  // Default fallback
  return M1Shield.convertColor(0xFFFF);  // White
}

int SignalOscilloscope::getSignalsOnCurrentPage() const {
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