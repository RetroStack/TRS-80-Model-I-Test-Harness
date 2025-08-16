#ifndef SIGNAL_OSCILLOSCOPE_H
#define SIGNAL_OSCILLOSCOPE_H

#include <ContentScreen.h>

class SignalOscilloscope : public ContentScreen {
 public:
  SignalOscilloscope();
  void loop() override;
  Screen* actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _drawContent() override;

 private:
  // Signal definitions - ALL available signals from getStateData()
  static const int SIGNAL_COUNT = 36;     // 16 addr + 8 data + 7 memory + 5 system = 36 total
  static const int UPDATE_INTERVAL = 20;  // ms between updates - faster for responsiveness
  static const int SIGNALS_PER_PAGE = 8;  // Max signals per page (except first page shows all)

  // Signal names and their current states
  const char* _signalNames[SIGNAL_COUNT] = {
      "A15", "A14",  "A13", "A12",   // Address bus high bits
      "A3",  "A2",   "A1",  "A0",    // Address bus low bits
      "D7",  "D6",   "D5",  "D4",    // Data bus high bits
      "D3",  "D2",   "D1",  "D0",    // Data bus low bits
      "RD",  "WR",   "IN",  "OUT",   // Memory control signals
      "RAS", "CAS",  "MUX", "WAIT",  // System control signals
      "RST", "IACK", "INT", "TEST"   // System status signals
  };

  // Current position in the rolling display
  int _plotPosition;  // Current X position for drawing

  // Paging support
  int _currentPage;  // Current page number (0-based)
  int _totalPages;   // Total number of pages

  // Timing and state tracking
  unsigned long _lastUpdate;
  bool _needsFullRedraw;
  bool _isRunning;

  // Private methods
  void drawSignalLabels();
  void clearPlotColumn(int x);
  void drawSignalColumn(int x, uint64_t stateData);
  uint16_t getSignalColor(int signalIndex, bool state);
  bool extractSignalFromState(uint64_t stateData, int signalIndex);

  // Page management
  int getSignalsOnCurrentPage() const;
  int getFirstSignalOnCurrentPage();
};

#endif  // SIGNAL_OSCILLOSCOPE_H
