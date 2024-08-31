// RetroStack Model 1 Test Harness

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "memory.h"
#include "memory_tests.h"
#include "roms.h"
#include "shield_pins.h"
#include "video.h"
#include "utils.h"

Set memoryChipSet[2];   // 0 = SRAM, 1 = DRAM

bool isBUSAKWired = false;          // BUSAK* connected to Test Harness?
char floatValString[20];            // storage for float printing
char splashMsg[] = "RETROSTACK M1 TEST HARNESS - BETA V";
float version = 0.15;               // Version

// Function declarations
void handle1();
void handleI();
void handleM();
void handleR();
void handleS();
void handleT();
void handleV();
void handleX();
void printMainMenu();
void printLine(const __FlashStringHelper* line);
extern void keyboardUtilities();

void setup() {
  // Initialize serial communication at 500000 baud rate
  Serial.begin(500000);
  Serial.println("\n");
  Serial.print(splashMsg);
  Serial.println(version, 3); // Print the version with 3 decimal places
  inGlobalTestMode = false;
  initControlPins();
  setAddressLinesToInput();
  setDataLinesToInput();
  printLine(F("Setup complete."));

  handle1();
  displaySplash();

}

void loop() {
    static char inputString[32]; // Buffer to hold incoming data
    static bool stringComplete = false; // Whether the string is complete
    static uint8_t index = 0; // Current index in the buffer

    // Read the serial input:
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\r' || inChar == '\n' || index >= sizeof(inputString) - 1) { // Check for CR, LF, or buffer overflow
            inputString[index] = '\0'; // Null-terminate the string
            stringComplete = true;
            index = 0; // Reset index for next input
        } else {
            inputString[index++] = inChar; // Add character to buffer
        }
    }

    // Process the input when a complete string has been received
    if (stringComplete) {
      switch (inputString[0]) {
        case 0:
          break;
        case '0':
          printLine(F("Clear screen."));
          cls();
          break;
        case '1':
          handle1();
          break;
        case '2':
          handle2();
          break;
        case 'b':
          handleB();
          break;
        case 'f':
          handle_f();
          break;
        case 'i':
          handleI();
          break;
        case 'k':
          handleK();
          break;
        case 'm':
          handleM();
          break;
        case 'p':
          handleP();
          break;
        case 'r':
          handleR();
          break;
        case 's':
          handleS();
          break;
        case 'v':
          handleVideoMenu();
          // printMainMenu();
          break;
        case 'x':
          handleX();
          break;
        case '?':
          printMainMenu();
          break;
        default:
          printLine(F("Invalid input: "), inputString);
          // Serial.println((unsigned char)*inputString, HEX);
          break;
      }

      // Clear the buffer and reset the state
      // Serial.println("--clearing main menu buffer--");
      memset(inputString, 0, sizeof(inputString));
      stringComplete = false;
      index = 0; // Reset index for next input

      serialFlush();
      Serial.println(F("Input choices: 1, c, i, k, m, r, s, t, v, x or ? for help."));
    }
}

// Toggle TEST* pin and Global Test Mode flag
void handle1() {
  inGlobalTestMode = !inGlobalTestMode;
  setTESTPin(inGlobalTestMode ? LOW : HIGH);
  Serial.print(F("Setting Global Test Mode flag to "));
  Serial.println(inGlobalTestMode ? F("true.") : F("false."));
}

// Display TRS-80 control bus line statuses
void handle2() {
  displayCtrlPinStatus();
}

// Toggle on/off flag to indicate if BUSAK* is connected to Test Harness
void handleB() {
  printLine(F("Setting BUSAK* wired to Test Harness flag to "));

  isBUSAKWired = !isBUSAKWired;
  Serial.println(isBUSAKWired ? F(" true.") : F(" false."));
  if (!isBUSAKWired) {
    setTESTPin(HIGH);
  }
}

// Show Test Harness flags
void handle_f() {
  Serial.println(F("Test Harness Flags: "));
  sprintf(stringBuffer, "\tGlobal Test Mode: %s", inGlobalTestMode ? "On" : "Off");
  printLine(stringBuffer);
  sprintf(stringBuffer, "\tBUSAK* connected: %s\n", isBUSAKWired ? "Yes" : "No");
  printLine(stringBuffer);
}

// Initalize Test Harness ports and pin
void handleI() {
    Serial.print(F("Initializing Test Harness ports and pins and resetting Global Test Flag."));
    initControlPins();
    inGlobalTestMode = false;
    setAddressLinesToInput();
    setDataLinesToInput();
}

// Keyboard utilities
void handleK() {
  keyboardUtilities();
}

void handleM() {
    printMemoryMenu();
    while (true) {
        if (Serial.available()) {
            char inChar = (char)Serial.read();
            if (inChar == 'x') {
                printMainMenu();
                break;
            } else if (inChar == '1') {
              readVRAM();
            } else if (inChar == '2') {
                memoryTest2();
            } else if (inChar == '?') {
                printMemoryMenu();
            } else {
                Serial.println(F("Invalid input. Choose 1, 2, or x to return to main menu."));
            }
        }
    }
}

void handleP() {
  Serial.println(F("Port stuff - cassette and video - TBD"));
}

void handleR() {
  // TODO: needs to support global test mode
  Serial.println(F("ROM checksum and version ID"));

  noInterrupts();
  cls();

  displayString(0x3c00, "ROM CHECKSUM AND VERSION");

  // enterTestMode();
  getROMInfo(false);
  // exitTestMode();

  sprintf(stringBuffer, "%s%lX", "CHECKSUM: ", romChecksum);  
  displayString(0x3C80, stringBuffer);
  sprintf(stringBuffer, "%s%s", " VERSION: ", romVersion);
  displayString(0x3CC0, stringBuffer);

  interrupts();
}

// Show system configuration
void handleS() {
  bool hasLowercase = false;
  printLine(F("--- System configuration ---\n"), F("Identifying..."));
  getROMInfo(true);
  printLine("ROM version: ", romVersion);
  Serial.print("ROM checksum: ");
  Serial.println(romChecksum, HEX);
  hasLowercase = lowercaseModExists();
  Serial.print(F("Lowercase mod: "));
  Serial.println(hasLowercase ? F("Yes") : F("Not found"));
}

void handleX() {
    Serial.println(F("Handled input: x"));
    exitTestMode();
    printMainMenu();
}

void printMainMenu() {
    Serial.println(F("*** Main Menu ***"));
    Serial.println(F("\t0 - Clear video screen"));    
    Serial.println(F("\t1 - Toggle TEST* pin and Global Test Mode flag"));
    Serial.println(F("\t2 - Show control TRS-80 bus pin statuses"));
    Serial.println(F("\tb - BUSAK* connected to Test Harness (WIP)"));
    Serial.println(F("\tc - Cassette utilities (WIP)"));
    Serial.println(F("\tf - Test Harness flags"));
    Serial.println(F("\ti - Reinit Test Harness ports and pins"));
    Serial.println(F("\tk - Keyboard utilities"));
    Serial.println(F("\tm - Memory (DRAM) utilities (WIP)"));
    Serial.println(F("\tr - ROM checksum and version ID"));
    Serial.println(F("\tp - TRS-80 Port utilities"));
    Serial.println(F("\ts - System configuration"));
    Serial.println(F("\tv - Video utilities"));
    Serial.println(F("\tx - Test mode off, TEST* driven high; data, address and control signals set to inputs."));
    Serial.println(F("\t? - Prints this help menu"));
}

void printLine(const __FlashStringHelper* line) {
    Serial.println(line);
}

void displaySplash() {
  cls();
  dtostrf(version, 4, 2, floatValString);
  sprintf(stringBuffer, "%s%s", splashMsg, floatValString);
  displayString(0x3c00, stringBuffer);
  printLine(stringBuffer);
  printMainMenu(); 
}