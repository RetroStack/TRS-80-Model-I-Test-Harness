#ifndef KEYBOARD_TESTER_H
#define KEYBOARD_TESTER_H

#include <ContentScreen.h>
#include <Keyboard.h>

class KeyboardTester : public ContentScreen {
 public:
  enum DisplayMode {
    MODE_GRAPHICAL = 0,  // Visual TRS-80 keyboard layout
    MODE_MATRIX = 1,     // 8x8 matrix grid view
  };

 private:
  // Key testing tracking - using matrix coordinates [row][col]
  bool _keyTested[8][8];       // Track which keys have been pressed in the past (8x8 matrix)
  bool _keyJustPressed[8][8];  // Track just pressed state for change detection
  unsigned long _keyHighlightTime[8][8];  // Track when keys were pressed for highlight timing

  unsigned long _lastUpdate;
  DisplayMode _currentMode;
  bool _needsFullRedraw;  // Flag to indicate full redraw is needed

 public:
  KeyboardTester();
  bool open() override;
  void close() override;
  void loop() override;
  Screen *actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) override;

 protected:
  void _drawContent() override;

 private:
  void updateModeTitle();
  void displayKeyboardStatus();

  // Helper Graphical Keyboard
  void processGraphicalKeyboard(bool fullDraw);
  void calculateGraphicalLayout(int &startX, int &startY, int &keyWidth, int &keyHeight);
  void drawKeyboardRow(const char *keys[], int keyCount, int keyMatrix[][2], int startX, int y,
                       int keyWidth, int keyHeight);
  void updateKeyboardRow(const char *keys[], int keyCount, int keyMatrix[][2], int startX, int y,
                         int keyWidth, int keyHeight, bool &anyKeyChanged);
  void drawKey(int x, int y, int width, int height, const char *label, bool tested,
               bool highlighted);

  // Helper Matrix Keyboard
  void processMatrixView(bool fullDraw);
  String getMatrixKeyName(uint8_t row, uint8_t col);
  void calculateMatrixLayout(int &startX, int &startY, int &cellWidth, int &cellHeight);
  void drawMatrixCell(int x, int y, int width, int height, uint8_t row, uint8_t col, String keyChar,
                      bool tested, bool highlighted);
};

#endif  // KEYBOARD_TESTER_H
