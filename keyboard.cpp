#include "keyboard.h"
#include "video.h"

// Look-up table for uppercase characters
const char* lookupTable[16][8] = {
  {"@", "A", "B", "C", "D", "E", "F", "G"},       // 3801
  {"H", "I", "J", "K", "L", "M", "N", "O"},       // 3802
  {"P", "Q", "R", "S", "T", "U", "V", "W"},       // 3804
  {"X", "Y", "Z", "S1", "S2", "S3", "S4", "S5"},            // 3808
  {"0", "1", "2", "3", "4", "5", "6", "7"},       // 3810
  {"8", "9", ":", ";", ",", "-", ".", "/"},       // 3820
  {"ENTER", "CLEAR", "BREAK", "UP", "DOWN", "LEFT", "RIGHT", "SPACEBAR"}, // 3840
  {"SHIFT", "", "", "", "CONTROL", "", "", ""},   // 3880
  {"@", "a", "b", "c", "d", "e", "f", "g"},       // 3801
  {"h", "i", "j", "k", "l", "m", "n", "o"},       // 3802
  {"p", "q", "r", "s", "t", "u", "v", "w"},       // 3804
  {"x", "y", "z", "", "", "", "", ""},            // 3808
  {"0", "!", "\"", "#", "$", "%", "&", "'"},       // 3810
  {"(", ")", "*", "+", "<", "=", ">", "?"},       // 3820
  {"Enter", "Clear", "Break", "Up", "Down", "Left", "Right", "Spacebar"}, // 3840
  {"Shift", "", "", "", "Control", "", "", ""},   // 3880
};

void handleK1();
void handleS1();

// TODO: NEED TO remove VRAM read

// TODO: need to support this
bool hasLowercase = true;

void loopKeyboard() {
  setAddressLinesToOutput(KEYBOARD_MEM_ADDRESS);
  setDataLinesToInput();

  for (int i = 0; i < 8; i++) {
    uint16_t keyMemAddress = KEYBOARD_MEM_ADDRESS + (1 << i);
    bool shiftPressed = false;

    // Serial.println(keyMemAddress, HEX);
    // uint8_t keyCode = readByteMemory(keyMemAddress);

    // Check if Shift is pressed
    uint8_t keyCode = readByteVRAM(keyMemAddress);    
    if ((keyMemAddress == 0x3880) && (keyCode & 0x01)) {
      shiftPressed = true;
    }
    
    for (int bit = 0; bit < 8; bit++) {
      if (keyCode & (1 << bit)) {
        Serial.print(keyCode, HEX);
        Serial.print("  ");

        if (!shiftPressed) {
          Serial.print(lookupTable[i][bit]);
        } else {
          Serial.print(lookupTable[i + 8][bit]);
        }
        Serial.println(" ");
        asmWait(50000);
      }
    }
  }
}


// TODO: support SHIFT key to show lowercase
// TODO: support CONTROL key
void handleK1() {
  printLine(F("Key test"));

  setAddressLinesToOutput(KEYBOARD_MEM_ADDRESS);
  setDataLinesToInput();

  printLine(F("Pressed key will be shown in console if it is working."));
  printLine(F("Press <BREAK> on TRS-80 keyboard to end session - else restart Test Harness."));
  inputPrompt("Press <ENTER> to start test.");

  printLine("Address |   Bits   | Keys");
  bool breakTest = false;

  while (!breakTest) {
    for (int i = 0; i < 8; i++) {
      uint16_t keyMemAddress = KEYBOARD_MEM_ADDRESS + (1 << i);
      uint8_t keyCode = readByteVRAM(keyMemAddress);

      for (int bit = 0; bit < 8; bit++) {
        if (keyCode & (1 << bit)) {
          sprintfBinary(keyCode);
          sprintf(stringBuffer, "  %04Xh | %s | ", keyMemAddress, binaryString);
          Serial.print(stringBuffer);

          Serial.println(lookupTable[i][bit]);

          if ((keyMemAddress == 0x3840) && (bit == 2)) {
            printLine(F("<BREAK> key pressed, ending test."));
            breakTest = true;
          } else {
            asmWait(65535, 25);
          }

          break;
        }
      }
    }
  }
}


// TODO: pressing Z,X,B yields: @BXZ - why?? @marcel
void handleS1() {
  printLine(F("Scan keyboard for stuck keys"));

  setAddressLinesToOutput(KEYBOARD_MEM_ADDRESS);
  setDataLinesToInput();

  printLine("Do not touch the TRS-80 keyboard.");
  inputPrompt("Stuck keys will be shown in console. Press <ENTER> to start check.");

  bool stuckKeyFound = false;
  printLine("Address |   Bits   | Keys");  
  for (int i = 0; i < 8; i++) {
    uint16_t keyMemAddress = KEYBOARD_MEM_ADDRESS + (1 << i);

    uint8_t keyCode = readByteVRAM(keyMemAddress);

    sprintfBinary(keyCode);
    sprintf(stringBuffer, "  %04Xh | %s | ", keyMemAddress, binaryString);
    Serial.print(stringBuffer);

    for (int bit = 0; bit < 8; bit++) {
      if (keyCode & (1 << bit)) {
        stuckKeyFound = true;
        Serial.print(lookupTable[i][bit]);
        Serial.print(" ");
      }
    }
    printLine(" ");
  }

  if (!stuckKeyFound) {
    printLine("No stuck keys found.");
  }
}

void keyboardUtilities() {
  printKeyboardUtilitiesMenu();

  while (true) {
    if (Serial.available()) {
      char inChar = (char)Serial.read();
      if (inChar == '0') {
        cls();
      } else if (inChar == 'k') {
        handleK1();
      } else if (inChar == 's') {
        handleS1();
      } else if (inChar == 'x') {
          break;
      } else if (inChar == '?') {
          printKeyboardUtilitiesMenu();
      } else {
          Serial.println(F("Invalid input. Choose 0, k, s, or x to return to main menu."));
      }
      serialFlush();
    }
  }
  Serial.println("Returning to main menu.");
  serialFlush();
}

void printKeyboardUtilitiesMenu() {
    Serial.println(F("*** Keyboard Utilities ***"));
    Serial.println(F("\t0 - Clear screen"));
    Serial.println(F("\tk - Key test"));
    Serial.println(F("\ts - Stuck keys check"));
    Serial.println(F("\tx - Return to main menu"));
    Serial.println(F("\t? - Prints this menu"));
}



