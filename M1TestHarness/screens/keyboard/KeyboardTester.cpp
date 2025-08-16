#include "./KeyboardTester.h"

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <M1Shield.h>
#include <Model1.h>

#include "../../globals.h"
#include "../MainMenu.h"

#define MODE_COUNT 2
#define HIGHLIGHT_DURATION 500
#define CHECK_DELAY 100

KeyboardTester::KeyboardTester() : ContentScreen() {
  _lastUpdate = 0;
  _currentMode = MODE_GRAPHICAL;
  _needsFullRedraw = true;

  // Initialize key testing tracking arrays
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      _keyTested[i][j] = false;
      _keyJustPressed[i][j] = false;
      _keyHighlightTime[i][j] = 0;
    }
  }

  updateModeTitle();

  const __FlashStringHelper *buttons[] = {F("M:Menu"), F("RT:Reset"), F("UP:Prev"), F("DN:Next")};
  setButtonItemsF(buttons, 4);
}

void KeyboardTester::updateModeTitle() {
  switch (_currentMode) {
    case MODE_GRAPHICAL:
      setTitleF(F("Graphical Test"));
      break;
    case MODE_MATRIX:
      setTitleF(F("Matrix Test"));
      break;
  }
}

bool KeyboardTester::open() {
  // Call parent implementation
  bool result = ContentScreen::open();

  // Activate test signal to allow keyboard access
  Model1.activateTestSignal();
  Globals.keyboard.update();  // Keep keyboard state current

  // Force a full redraw when opening
  _needsFullRedraw = true;

  return result;
}

void KeyboardTester::close() {
  // Deactivate test signal when screen is closed
  Model1.deactivateTestSignal();

  // Call parent implementation
  ContentScreen::close();
}

void KeyboardTester::loop() {
  unsigned long currentTime = millis();

  // Update keyboard status every CHECK_DELAY ms for responsive key detection
  if (currentTime - _lastUpdate > CHECK_DELAY) {
    _lastUpdate = currentTime;
    M1Shield.setLEDColor(COLOR_OFF);
    displayKeyboardStatus();  // Update status display and track key presses
  }

  // If we need a redraw, call _drawContent directly here in the loop
  // This seems to be the pattern that works with the framework
  if (_needsFullRedraw) {
    _drawContent();
  }
}

void KeyboardTester::displayKeyboardStatus() {
  unsigned long currentTime = millis();

  // Reset just pressed state; will be set in the next section if it was just pressed
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      _keyJustPressed[row][col] = false;
    }
  }

  Globals.logger.info("Key checked");

  // Use keyboard iterator to get all key changes
  KeyboardChangeIterator it = Globals.keyboard.changes();
  bool anyPressed = false;
  while (it.hasNext()) {
    uint8_t row = it.row();
    uint8_t col = it.column();
    uint8_t keyValue = it.keyValue();

    // Update the key state directly for this specific position
    if (row < 8 && col < 8) {
      bool isPressed = it.isPressed();
      if (isPressed) {
        Globals.logger.info("Key pressed");
        anyPressed = true;
        _keyHighlightTime[row][col] = currentTime;
      } else {
        if (currentTime - _keyHighlightTime[row][col] > HIGHLIGHT_DURATION) {
          _keyHighlightTime[row][col] = 0;
        }
      }

      // Mark key as just-pressed and tested when pressed
      if (it.wasJustPressed()) {
        _keyTested[row][col] = true;
        _keyJustPressed[row][col] = true;
      }
    }

    // Log key activity
    if (it.wasJustPressed()) {
      String keyName = it.keyName();
      Globals.logger.infoF(F("Key pressed: %s (0x%s) at row %d, col %d"), keyName,
                           String(keyValue, HEX), row, col);

    } else if (it.wasReleased()) {
      String keyName = it.keyName();
      Globals.logger.infoF(F("Key released: %s (0x%s) at row %d, col %d"), keyName,
                           String(keyValue, HEX), row, col);
    }

    it.next();
  }

  if (anyPressed) {
    M1Shield.setLEDColor(COLOR_WHITE);
  }

  _drawContent();
}

void KeyboardTester::_drawContent() {
  // Only draw when screen is active
  if (!isActive())
    return;

  Adafruit_GFX &gfx = M1Shield.getGFX();
  gfx.startWrite();

  if (_needsFullRedraw) {
    // Clear content area before full redraw
    uint16_t contentLeft = _getContentLeft();
    uint16_t contentTop = _getContentTop();
    uint16_t contentWidth = _getContentWidth();
    uint16_t contentHeight = _getContentHeight();

    gfx.fillRect(contentLeft, contentTop, contentWidth, contentHeight,
                 M1Shield.convertColor(0x0000));

    switch (_currentMode) {
      case MODE_GRAPHICAL:
        processGraphicalKeyboard(true);
        break;
      case MODE_MATRIX:
        processMatrixView(true);
        break;
    }

    _needsFullRedraw = false;
  } else {
    // Only update keys that have changed state
    switch (_currentMode) {
      case MODE_GRAPHICAL:
        processGraphicalKeyboard(false);
        break;
      case MODE_MATRIX:
        processMatrixView(false);
        break;
    }
  }

  gfx.endWrite();
}

// -----------------------------------------------
// GRAPHICAL FUNCTIONS
// -----------------------------------------------

void KeyboardTester::processGraphicalKeyboard(bool fullDraw) {
  int startX, startY, keyWidth, keyHeight;
  calculateGraphicalLayout(startX, startY, keyWidth, keyHeight);

  // Define key layout arrays and matrix mappings (shared between full draw and updates)
  const char *row1[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", ":", "-", "BRK"};
  const char *row2[] = {"UP", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "@", "LF", "RT"};
  const char *row3[] = {"DN", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "ENT", "CLR"};
  const char *row4[] = {"SHF", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "SHF"};
  const char *row5[] = {"SPACE"};

  int keyMatrix1[][2] = {{4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7},
                         {5, 0}, {5, 1}, {4, 0}, {5, 2}, {5, 5}, {6, 2}};
  int keyMatrix2[][2] = {{6, 3}, {2, 1}, {2, 7}, {0, 5}, {2, 2}, {2, 4}, {3, 1},
                         {2, 5}, {1, 1}, {1, 7}, {2, 0}, {0, 0}, {6, 5}, {6, 6}};
  int keyMatrix3[][2] = {{6, 4}, {0, 1}, {2, 3}, {0, 4}, {0, 6}, {0, 7}, {1, 0},
                         {1, 2}, {1, 3}, {1, 4}, {5, 3}, {6, 0}, {6, 1}};
  int keyMatrix4[][2] = {{7, 0}, {3, 2}, {3, 0}, {0, 3}, {2, 6}, {0, 2},
                         {1, 6}, {1, 5}, {5, 4}, {5, 6}, {5, 7}, {7, 0}};
  int keyMatrix5[][2] = {{6, 7}};

  bool anyKeyChanged = false;

  if (fullDraw) {
    drawKeyboardRow(row1, 13, keyMatrix1, startX + 25, startY, keyWidth, keyHeight);
    drawKeyboardRow(row2, 14, keyMatrix2, startX + 0, startY + 25, keyWidth, keyHeight);
    drawKeyboardRow(row3, 13, keyMatrix3, startX + 10, startY + 50, keyWidth, keyHeight);
    drawKeyboardRow(row4, 12, keyMatrix4, startX + 10, startY + 75, keyWidth, keyHeight);
    drawKeyboardRow(row5, 1, keyMatrix5, startX + 100, startY + 100, 120, keyHeight);
    M1Shield.display();

  } else {  // Only changes
    updateKeyboardRow(row1, 13, keyMatrix1, startX + 25, startY, keyWidth, keyHeight,
                      anyKeyChanged);
    updateKeyboardRow(row2, 14, keyMatrix2, startX + 0, startY + 25, keyWidth, keyHeight,
                      anyKeyChanged);
    updateKeyboardRow(row3, 13, keyMatrix3, startX + 10, startY + 50, keyWidth, keyHeight,
                      anyKeyChanged);
    updateKeyboardRow(row4, 12, keyMatrix4, startX + 10, startY + 75, keyWidth, keyHeight,
                      anyKeyChanged);
    updateKeyboardRow(row5, 1, keyMatrix5, startX + 100, startY + 100, 120, keyHeight,
                      anyKeyChanged);

    if (anyKeyChanged) {
      M1Shield.display();
    }
  }
}

void KeyboardTester::calculateGraphicalLayout(int &startX, int &startY, int &keyWidth,
                                              int &keyHeight) {
  // Get content area bounds
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentWidth = _getContentWidth();
  uint16_t contentHeight = _getContentHeight();

  // Define key dimensions and spacing - sized to fit in available space
  // Available width: 318px, need to fit 14 keys max
  keyWidth = 20;
  keyHeight = 18;
  const int keySpacing = 2;

  // Calculate keyboard dimensions for centering
  const int totalKeyboardWidth =
      14 * (keyWidth + keySpacing) - keySpacing;             // 14 keys in longest row
  const int totalKeyboardHeight = 5 * (keyHeight + 5) + 20;  // 4 rows + spacebar + spacing

  // Center the keyboard in the content area
  int tempStartX = contentLeft + (contentWidth - totalKeyboardWidth) / 2;
  int tempStartY = contentTop + (contentHeight - totalKeyboardHeight) / 2;

  // Safety check: ensure coordinates are not negative
  startX = (tempStartX < (int)contentLeft) ? contentLeft : tempStartX;
  startY = (tempStartY < (int)contentTop) ? contentTop : tempStartY;
}

void KeyboardTester::drawKeyboardRow(const char *keys[], int keyCount, int keyMatrix[][2],
                                     int startX, int y, int keyWidth, int keyHeight) {
  unsigned long currentTime = millis();

  for (int i = 0; i < keyCount; i++) {
    int x = startX + i * (keyWidth + 2);  // 2 pixel spacing between keys

    // Get matrix position for this key
    int matrixByte = keyMatrix[i][0];
    int matrixBit = keyMatrix[i][1];
    bool isTested = false;
    bool isHighlighted = false;

    if (matrixByte >= 0 && matrixBit >= 0 && matrixByte < 8 && matrixBit < 8) {
      isTested = _keyTested[matrixByte][matrixBit];

      if (_keyHighlightTime[matrixByte][matrixBit] != 0) {
        isHighlighted =
            (currentTime - _keyHighlightTime[matrixByte][matrixBit] < HIGHLIGHT_DURATION);
        if (!isHighlighted) {
          _keyHighlightTime[matrixByte][matrixBit] = 0;
        }
      }
    }

    // Draw key with current state (highlighting happens when key is pressed)
    drawKey(x, y, keyWidth, keyHeight, keys[i], isTested, isHighlighted);
  }
}

void KeyboardTester::updateKeyboardRow(const char *keys[], int keyCount, int keyMatrix[][2],
                                       int startX, int y, int keyWidth, int keyHeight,
                                       bool &anyKeyChanged) {
  unsigned long currentTime = millis();

  for (int i = 0; i < keyCount; i++) {
    int x = startX + i * (keyWidth + 2);  // 2 pixel spacing between keys

    // Get matrix position for this key
    int matrixByte = keyMatrix[i][0];
    int matrixBit = keyMatrix[i][1];

    if (matrixByte >= 0 && matrixBit >= 0 && matrixByte < 8 && matrixBit < 8) {
      // Check if this key's state has changed
      bool currentlyPressed = _keyJustPressed[matrixByte][matrixBit];
      bool highlightChanged = false;
      bool isHighlighted = false;

      if (_keyHighlightTime[matrixByte][matrixBit] != 0) {
        isHighlighted =
            (currentTime - _keyHighlightTime[matrixByte][matrixBit] < HIGHLIGHT_DURATION);
        if (!isHighlighted) {
          _keyHighlightTime[matrixByte][matrixBit] = 0;
          highlightChanged = true;
        }
      }

      if (currentlyPressed || highlightChanged) {
        anyKeyChanged = true;
        bool isTested = _keyTested[matrixByte][matrixBit];

        // Redraw only this key with its new state
        drawKey(x, y, keyWidth, keyHeight, keys[i], isTested, isHighlighted);
      }
    }
  }
}

void KeyboardTester::drawKey(int x, int y, int width, int height, const char *label, bool tested,
                             bool highlighted) {
  Adafruit_GFX &gfx = M1Shield.getGFX();

  // Determine colors based on key state
  uint16_t bgColor, textColor, borderColor = 0x5AEB;  // Light gray border
  if (highlighted) {
    // Currently pressed - white background, black text
    bgColor = 0xFFFF;    // White
    textColor = 0x0000;  // Black
  } else if (tested) {
    // Key has been tested - dark green background, white text
    bgColor = 0x0320;    // Dark green
    textColor = 0xFFFF;  // White
  } else {
    // Key not yet tested - dark red background, white text
    bgColor = 0x8000;    // Dark red
    textColor = 0xFFFF;  // White
  }

  // Draw key rectangle with border
  gfx.fillRect(x, y, width, height, M1Shield.convertColor(bgColor));
  gfx.drawRect(x, y, width, height, M1Shield.convertColor(borderColor));

  // Draw key label centered
  gfx.setTextColor(M1Shield.convertColor(textColor));
  gfx.setTextSize(1);

  // Calculate text position to center it in the key
  int textWidth = strlen(label) * 6;  // Approximate character width
  int textX = x + (width - textWidth) / 2;
  int textY = y + (height - 8) / 2;  // 8 is approximate character height

  gfx.setCursor(textX, textY);
  gfx.print(label);
}

// -----------------------------------------------
// MATRIX FUNCTIONS
// -----------------------------------------------

void KeyboardTester::processMatrixView(bool fullDraw) {
  int startX, startY, cellWidth, cellHeight;
  calculateMatrixLayout(startX, startY, cellWidth, cellHeight);

  static const char *keyMatrix[8][8] = {
      {"@", "A", "B", "C", "D", "E", "F", "G"},             // Row 0 (0x3801)
      {"H", "I", "J", "K", "L", "M", "N", "O"},             // Row 1 (0x3802)
      {"P", "Q", "R", "S", "T", "U", "V", "W"},             // Row 2 (0x3804)
      {"X", "Y", "Z", "", "", "", "", ""},                  // Row 3 (0x3808) - unused positions 3-7
      {"0", "1", "2", "3", "4", "5", "6", "7"},             // Row 4 (0x3810)
      {"8", "9", ":", ";", ",", "-", ".", "/"},             // Row 5 (0x3820)
      {"ENT", "CLR", "BRK", "UP", "DN", "LF", "RT", "SP"},  // Row 6 (0x3840) - special keys
      {"SHF", "", "", "", "", "", "", ""}                   // Row 7 (0x3860) - shift and unused
  };

  Adafruit_GFX &gfx = M1Shield.getGFX();
  bool anyKeyChanged = false;

  unsigned long currentTime = millis();

  if (fullDraw) {
    // Draw column headers (0-7)
    for (int col = 0; col < 8; col++) {
      int x = startX + col * cellWidth + cellWidth / 2 - 3;
      gfx.setCursor(x, startY - 15);
      gfx.setTextColor(M1Shield.convertColor(0x07FF));
      gfx.setTextSize(1);
      gfx.print(col);
    }

    // Draw row headers and all matrix cells
    for (int row = 0; row < 8; row++) {
      int y = startY + row * cellHeight;

      // Row header
      gfx.setCursor(startX - 15, y + 6);
      gfx.setTextColor(M1Shield.convertColor(0x07FF));
      gfx.setTextSize(1);
      gfx.print(row);

      for (int col = 0; col < 8; col++) {
        int x = startX + col * cellWidth;
        bool isTested = _keyTested[row][col];
        bool isHighlighted = false;

        if (_keyHighlightTime[row][col] != 0) {
          isHighlighted = (currentTime - _keyHighlightTime[row][col] < HIGHLIGHT_DURATION);
          if (!isHighlighted) {
            _keyHighlightTime[row][col] = 0;
          }
        }

        drawMatrixCell(x, y, cellWidth - 1, cellHeight - 1, row, col, keyMatrix[row][col], isTested,
                       isHighlighted);
      }
    }
    M1Shield.display();

  } else {
    // Check each matrix cell for changes
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        bool currentlyPressed = _keyJustPressed[row][col];
        bool highlightChanged = false;
        bool isHighlighted = false;

        if (_keyHighlightTime[row][col] != 0) {
          isHighlighted = (currentTime - _keyHighlightTime[row][col] < HIGHLIGHT_DURATION);
          if (!isHighlighted) {
            _keyHighlightTime[row][col] = 0;
            highlightChanged = true;
          }
        }

        if (currentlyPressed || highlightChanged) {
          anyKeyChanged = true;
          int x = startX + col * cellWidth;
          int y = startY + row * cellHeight;
          bool isTested = _keyTested[row][col];

          drawMatrixCell(x, y, cellWidth - 1, cellHeight - 1, row, col, keyMatrix[row][col],
                         isTested, isHighlighted);
        }
      }
    }

    if (anyKeyChanged) {
      M1Shield.display();
    }
  }
}

void KeyboardTester::calculateMatrixLayout(int &startX, int &startY, int &cellWidth,
                                           int &cellHeight) {
  // Get content area bounds
  uint16_t contentLeft = _getContentLeft();
  uint16_t contentTop = _getContentTop();
  uint16_t contentWidth = _getContentWidth();
  uint16_t contentHeight = _getContentHeight();

  // Matrix grid dimensions
  cellWidth = 30;
  cellHeight = 15;
  const int headerSpace = 18;  // Space for row/column headers

  // Calculate matrix dimensions for centering
  const int totalMatrixWidth = 8 * cellWidth + headerSpace;
  const int totalMatrixHeight = 8 * cellHeight + headerSpace;

  // Center the matrix in the content area
  int tempStartX = contentLeft + (contentWidth - totalMatrixWidth + headerSpace) / 2;
  int tempStartY = contentTop + (contentHeight - totalMatrixHeight + headerSpace) / 2;

  // Safety check: ensure coordinates are not negative
  startX = (tempStartX < (int)contentLeft) ? contentLeft : tempStartX;
  startY = (tempStartY < (int)contentTop) ? contentTop : tempStartY;
}

void KeyboardTester::drawMatrixCell(int x, int y, int width, int height, uint8_t row, uint8_t col,
                                    String keyChar, bool tested, bool highlighted) {
  Adafruit_GFX &gfx = M1Shield.getGFX();

  uint16_t bgColor, textColor, borderColor = 0xFFFF;

  if (highlighted) {
    // Currently pressed - white background, black text
    bgColor = 0xFFFF;    // White
    textColor = 0x0000;  // Black
  } else if (tested) {
    // Key has been tested - dark green background, white text
    bgColor = 0x0320;    // Dark green
    textColor = 0xFFFF;  // White
  } else {
    // Key not yet tested - dark red background, white text
    bgColor = 0x8000;    // Dark red
    textColor = 0xFFFF;  // White
  }

  // Draw cell rectangle with border
  gfx.fillRect(x, y, width, height, M1Shield.convertColor(bgColor));
  gfx.drawRect(x, y, width, height, M1Shield.convertColor(borderColor));

  // Draw key name centered in cell
  gfx.setTextColor(M1Shield.convertColor(textColor));
  gfx.setTextSize(1);

  int textWidth = keyChar.length() * 6;  // Approximate character width
  int textX = x + (width - textWidth) / 2;
  int textY = y + (height - 8) / 2;  // 8 is approximate character height

  gfx.setCursor(textX, textY);
  gfx.print(keyChar);
}

Screen *KeyboardTester::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_MENU) {  // MENU
    Globals.logger.infoF(F("Returning to main menu from Keyboard Tester"));
    return new MainMenu();
  }

  if (action & BUTTON_RIGHT) {  // RESET - Reset all key testing status
    Globals.logger.infoF(F("Resetting key test status"));
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        _keyTested[i][j] = false;
        _keyJustPressed[i][j] = false;
        _keyHighlightTime[i][j] = 0;
      }
    }
    _needsFullRedraw = true;  // Force full redraw after reset
    _drawContent();           // Redraw the screen with the new mode
  }

  if (action & UP_ANY) {  // Cycle to previous mode
    _currentMode = (DisplayMode)((_currentMode + MODE_COUNT - 1) % MODE_COUNT);
    updateModeTitle();
    _needsFullRedraw = true;  // Force full redraw for mode change
    _drawContent();           // Redraw the screen with the new mode
    Globals.logger.info(F("Switched to mode: %d"), _currentMode);
  }

  if (action & DOWN_ANY) {  // Cycle to next mode
    _currentMode = (DisplayMode)((_currentMode + 1) % MODE_COUNT);
    updateModeTitle();
    _needsFullRedraw = true;  // Force full redraw for mode change
    _drawContent();           // Redraw the screen with the new mode
    Globals.logger.info(F("Switched to mode: %d"), _currentMode);
  }

  return nullptr;
}
