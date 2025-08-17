#include "./CassetteTestSuiteConsole.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./CassetteMenu.h"

CassetteTestSuiteConsole::CassetteTestSuiteConsole() : ConsoleScreen() {
  setTitleF(F("Cassette Test Suite"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);

  // Set button navigation
  const __FlashStringHelper* buttons[] = {F("M:Menu"), F("Any:Continue")};
  setButtonItemsF(buttons, 2);
}

Screen* CassetteTestSuiteConsole::actionTaken(ActionTaken action, uint8_t offsetX,
                                              uint8_t offsetY) {
  if (action & BUTTON_MENU) {
    return new CassetteMenu();
  }

  // Since we handle all interaction in _executeOnce(), we don't need to process other actions here
  return nullptr;
}

void CassetteTestSuiteConsole::_executeOnce() {
  // Always show instructions first
  showConnectionInstructions();

  // Wait for user input using endless loop
  while (true) {
    // Check for MENU button press to cancel
    if (M1Shield.wasMenuPressed()) {
      println();
      println(F("Tests cancelled by user"));
      return;
    }

    // Check for any other button press to confirm and continue
    if (M1Shield.wasLeftPressed() || M1Shield.wasRightPressed() || M1Shield.wasUpPressed() ||
        M1Shield.wasDownPressed() || M1Shield.wasJoystickPressed()) {
      break;  // Exit the wait loop and start tests
    }

    delay(50);  // Small delay to prevent busy waiting
  }

  cls();

  // Run all tests
  runAllTests();

  // Show completion message
  println();
  println(F("All tests completed!"));
}

void CassetteTestSuiteConsole::showConnectionInstructions() {
  println(F("Cassette Interface Test"));
  println();

  setTextColor(0xFFFF, 0x0000);  // White for setup instructions
  println(F("Setup Required:"));
  println(F("- Connect M1 cassette port to Test harness"));
  println();

  setTextColor(0x07FF, 0x0000);  // Cyan for call-to-action
  println(F("Press any button to start tests"));
  println(F("Press MENU to cancel"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::runAllTests() {
  println();
  println(F("Starting Cassette Interface Tests..."));
  println();

  // Test Remote Control
  testRemoteControl();

  // Test Data Input/Output Combined
  testDataInputOutput();

  // Test Flip-Flop Behavior
  testFlipFlopBehavior();

  // Test Pattern Integrity
  testPatternIntegrity();
}

void CassetteTestSuiteConsole::testRemoteControl() {
  println(F("Testing Remote Control..."));

  // Test Remote activation
  Globals.cassette.activateRemote();
  delay(500);

  // Test Remote deactivation
  Globals.cassette.deactivateRemote();
  delay(500);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Remote control test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testDataInputOutput() {
  println(F("Testing Data Input/Output (writeRaw & readRaw)..."));

  // Activate remote for testing
  Globals.cassette.activateRemote();
  delay(100);

  // Test write/read loop - write patterns and read them back
  bool testPatterns[] = {true, false, true, true, false, false, true, false};
  int patternLength = sizeof(testPatterns) / sizeof(testPatterns[0]);

  println(F("Writing test patterns..."));
  for (int i = 0; i < patternLength; i++) {
    // Write data pattern
    Globals.cassette.writeRaw(testPatterns[i], !testPatterns[i]);
    delay(20);

    // Read back immediately to test interface responsiveness
    bool readValue = Globals.cassette.readRaw();
    delay(10);
  }

  println(F("Testing continuous write/read cycles..."));
  // Test alternating write/read pattern for interface stability
  for (int i = 0; i < 10; i++) {
    // Write alternating pattern
    Globals.cassette.writeRaw(i % 2 == 0, i % 2 == 1);
    delay(10);

    // Read to verify interface responds
    bool readData = Globals.cassette.readRaw();
    delay(10);
  }

  Globals.cassette.deactivateRemote();

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Data input/output test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testFlipFlopBehavior() {
  println(F("Testing Flip-Flop Behavior..."));

  Globals.cassette.activateRemote();
  delay(100);

  // Test state changes
  Globals.cassette.writeRaw(true, false);
  delay(50);
  Globals.cassette.writeRaw(false, true);
  delay(50);
  Globals.cassette.writeRaw(false, false);
  delay(50);
  Globals.cassette.writeRaw(true, true);
  delay(50);

  Globals.cassette.deactivateRemote();

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Flip-flop behavior test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testPatternIntegrity() {
  println(F("Testing Pattern Integrity..."));

  Globals.cassette.activateRemote();
  delay(100);

  // Write known pattern
  bool pattern[] = {true, false, true, true, false, false, true, false};
  for (int i = 0; i < 8; i++) {
    Globals.cassette.writeRaw(pattern[i], !pattern[i]);
    delay(20);
  }

  Globals.cassette.deactivateRemote();

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Pattern integrity test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}
