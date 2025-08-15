#include "keyboard_th.h"

#define KEY_ENTER 0xB0
#define KEY_CLEAR 0xB2
#define KEY_BREAK 0xB1
#define KEY_UP 0xDA    // Up arrow
#define KEY_DOWN 0xD9  // Down arrow
#define KEY_LEFT 0xD8  // Left arrow
#define KEY_RIGHT 0xD7 // Right arrow
#define KEY_SPACE 0x20 // Space character
#define KEY_SHIFT 0x81 // Shift key

// Unused keys, but make them distinguishable in lookup table
#define KEY_UNUSED_1 0xC2
#define KEY_UNUSED_2 0xC3
#define KEY_UNUSED_3 0xC4
#define KEY_UNUSED_4 0xC5
#define KEY_UNUSED_5 0xC6
#define KEY_UNUSED_6 0xC7
#define KEY_UNUSED_7 0xC8
#define KEY_UNUSED_8 0xC9
#define KEY_UNUSED_9 0xCA
#define KEY_UNUSED_10 0xCB
#define KEY_UNUSED_11 0xCC
#define KEY_UNUSED_12 0xCD

// Look-up table for uppercase characters
const uint8_t lookupTable[8][8] PROGMEM = {
    {'@', 'a', 'b', 'c', 'd', 'e', 'f', 'g'},                                                                         // 3801
    {'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o'},                                                                         // 3802
    {'p', 'q', 'r', 's', 't', 'u', 'v', 'w'},                                                                         // 3804
    {'x', 'y', 'z', KEY_UNUSED_1, KEY_UNUSED_2, KEY_UNUSED_3, KEY_UNUSED_4, KEY_UNUSED_5},                            // 3808
    {'0', '1', '2', '3', '4', '5', '6', '7'},                                                                         // 3810
    {'8', '9', ':', ';', ',', '-', '.', '/'},                                                                         // 3820
    {KEY_ENTER, KEY_CLEAR, KEY_BREAK, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE},                              // 3840
    {KEY_SHIFT, KEY_UNUSED_6, KEY_UNUSED_7, KEY_UNUSED_8, KEY_UNUSED_9, KEY_UNUSED_10, KEY_UNUSED_11, KEY_UNUSED_12}, // 3860
};
const uint8_t lookupTableShift[8][8] PROGMEM = {
    {'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G'},                                                                         // 3801
    {'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O'},                                                                         // 3802
    {'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W'},                                                                         // 3804
    {'X', 'Y', 'Z', KEY_UNUSED_1, KEY_UNUSED_2, KEY_UNUSED_3, KEY_UNUSED_4, KEY_UNUSED_5},                            // 3808
    {'0', '!', '"', '#', '$', '%', '&', '\''},                                                                        // 3810
    {'(', ')', '*', '+', '<', '=', '>', '?'},                                                                         // 3820
    {KEY_ENTER, KEY_CLEAR, KEY_BREAK, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE},                              // 3840
    {KEY_SHIFT, KEY_UNUSED_6, KEY_UNUSED_7, KEY_UNUSED_8, KEY_UNUSED_9, KEY_UNUSED_10, KEY_UNUSED_11, KEY_UNUSED_12}, // 3860
};

/* ----------------------------------------------------------------
 *  Public API
 * -------------------------------------------------------------- */
void KeyboardTH::showMenu()
{
  {
  println(TO_LCD,F("k - key test"));
  println(TO_LCD,F("s - stuck key test"));
  println(TO_LCD,F("x - Back to parent menu"));
  println(TO_LCD,F("B - Bus pin statuses"));
  println(TO_LCD,F("C - Clear screen"));
  printTestSignalState();
  println(TO_LCD,F("X - Reset Test Harness"));
  println(TO_LCD,F("? - help"));
}
}

/*  Dispatch input coming from Menu::pollSerial / pollButtons   */
void KeyboardTH::handleInput(const char* input)
{
  if (!input || strlen(input) != 1) {
    println(F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case 'k': keyTest(); break;
    case 's': stuckKeyTest(); break;
    case 'x': Menu::pop(); break;
    case '?': /* Menu system auto-shows */ break;
    default: println(F("Invalid input")); break;
  }
}

// Helper function to get text label for special key values (in flash)
const __FlashStringHelper *getKeyLabel(uint8_t key)
{
  switch (key)
  {
  case KEY_ENTER:
    return F("ENTER");
  case KEY_CLEAR:
    return F("CLEAR");
  case KEY_BREAK:
    return F("BREAK");
  case KEY_UP:
    return F("UP");
  case KEY_DOWN:
    return F("DOWN");
  case KEY_LEFT:
    return F("LEFT");
  case KEY_RIGHT:
    return F("RIGHT");
  case KEY_SPACE:
    return F("SPACE");
  case KEY_SHIFT:
    return F("SHIFT");
  default:
    return nullptr;
  }
}

// Helper to get the actual key symbol from row/col and shift state
char getKeySymbol(uint8_t row, uint8_t col, bool shift)
{
  if (row > 7 || col > 7)
    return '?';
  if (shift)
  {
    return (char)pgm_read_byte(&(lookupTableShift[row][col]));
  }
  else
  {
    return (char)pgm_read_byte(&(lookupTable[row][col]));
  }
}

void KeyboardTH::stuckKeyTest(uint16_t timeoutSeconds)
{

  printSeparator(TO_LCD, F("[KB] Stuck Key Test"), '-', 80, 0, 5);
  println(TO_LCD, F("Don't touch the keyboard for "), timeoutSeconds, F(" seconds. Press [BREAK] to exit early."));
  println(TO_LCD, F("Stuck key(s) details will be shown."));
  inputPrompt(TO_LCD, F("Press [ENTER] on PC to start. "));

  println(TO_LCD, F("Scanning..."));

  bool breakTest = false;
  keyboard.update();

  unsigned long startTime = millis();
  const unsigned long timeout = timeoutSeconds * 1000UL;

  bool foundStuckKey = false;
  while (!breakTest)
  {
    if (millis() - startTime >= timeout)
    {
      println(F("Test complete."));
      break;
    }
    KeyboardChangeIterator it = keyboard.changes();
    while (it.hasNext())
    {
      if (it.wasJustPressed())
      {
        foundStuckKey = true;
        uint8_t key = it.keyValue();
        uint8_t row = it.row();
        uint8_t col = it.column();
        bool shift = it.isShiftPressed();
        char symbol = getKeySymbol(row, col, shift);
        uint16_t address = 0x3800 + (1 << row);
        print(TO_LCD, F("Stuck key: Row "), row, F(", Col "), col, F(", Addr: 0x"), address, Hex, F(", ASCII: "), key, Dec, F(", Symbol: '"), symbol, F("'"));

        const __FlashStringHelper *label = getKeyLabel(key);
        if (label)
        {
          print(TO_LCD, F(" ["), label, F("]"));
        }
        println(TO_LCD);

        if (key == KEY_BREAK)
        {
          println(TO_LCD, F("<BREAK> key pressed, ending test."));
          breakTest = true;
        }
      }
      it.next();
    }
    // delay(50); // Small delay to avoid flooding output
  }
  if (!foundStuckKey)
  {
    println(F("No stuck keys were detected."));
  }

  serialFlush();
}

void KeyboardTH::keyTest()
{
  println(TO_LCD,("[KB] Key test"));
  println(TO_LCD, ("Pressed key will be shown in console if it is working."));
  println(TO_LCD, ("Press TRS-80 <BREAK> key to end test - else restart Test Harness."));
  inputPrompt(TO_LCD, ("Press [ENTER] on PC to start test, and start banging on the keys! "));
  println(TO_LCD, ("Scanning..."));

  bool breakTest = false;
  keyboard.update();
  bool foundKey = false;

  while (!breakTest)
  {
    KeyboardChangeIterator it = keyboard.changes();
    while (it.hasNext())
    {
      if (it.wasJustPressed())
      {
        foundKey = true;
        uint8_t key = it.keyValue();
        uint8_t row = it.row();
        uint8_t col = it.column();
        bool shift = it.isShiftPressed();
        char symbol = getKeySymbol(row, col, shift);
        uint16_t address = 0x3800 + (1 << row);
        print(TO_LCD, F("Key: Row "), row, F(", Col "), col, F(", Addr: 0x"), address, Hex, F(", ASCII: "), key, F(", Symbol: '"), symbol, F("'"));

        const __FlashStringHelper *label = getKeyLabel(key);
        if (label)
        {
          print(TO_LCD, F(" ["), label, F("]"));
        }
        println();

        if (key == KEY_BREAK)
        {
          println(F("<BREAK> key pressed, ending test."));
          breakTest = true;
        }
      }
      it.next();
    }
    // delay(50); // Small delay to avoid flooding output
  }
  if (!foundKey)
  {
    println(TO_LCD, F("No keys were detected during the test."));
  }

  serialFlush(); // Clear any remaining input
}
