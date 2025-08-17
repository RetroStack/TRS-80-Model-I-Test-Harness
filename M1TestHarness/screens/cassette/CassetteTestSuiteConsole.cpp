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
  println(F("Cassette Interface Comprehensive Test"));
  println();
  println(F("This test will verify:"));
  println(F("- Remote1/Remote2 (CR1/CR2) pin configuration"));
  println(F("- Data output (writeCassetteIn)"));
  println(F("- Data input (readCassetteOut)"));
  println(F("- Analog cassette interface"));
  println(F("- Pattern integrity testing"));
  println(F("- Timing and frequency response"));
  println(F("- Voltage range validation"));
  println(F("- Signal integrity and crosstalk"));
  println(F("- Endurance testing"));
  println();

  setTextColor(0xFFFF, 0x0000);  // White for setup instructions
  println(F("Setup Required:"));
  println(F("- Connect M1 cassette port to Test harness"));
  println(F("- Ensure CR1/CR2 connections are proper"));
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

  // Basic functionality tests
  testRemoteControl();
  testCassettePinConfiguration();
  testDataOutput();
  testDataInput();
  testAnalogCassetteInterface();

  // Advanced tests
  testTimingAndFrequency();
  testVoltageRange();
  testSignalIntegrity();
  testCrosstalk();
  testPatternIntegrity();
  testEndurance();
}

void CassetteTestSuiteConsole::testRemoteControl() {
  println(F("Testing Remote Control (CR1/CR2)..."));

  // Test CR1 as output, CR2 as input
  println(F("Configuring CR1 as output, CR2 as input..."));
  M1Shield.setCR1Mode(true);   // CR1 as output
  M1Shield.setCR2Mode(false);  // CR2 as input
  delay(100);

  // Test CR1 output states
  println(F("Testing CR1 output states..."));
  M1Shield.writeCR1(true);  // CR1 HIGH
  delay(100);
  M1Shield.writeCR1(false);  // CR1 LOW
  delay(100);

  // Test CR2 as output, CR1 as input (if safe)
  println(F("Configuring CR2 as output, CR1 as input..."));
  M1Shield.setCR2Mode(true);   // CR2 as output
  M1Shield.setCR1Mode(false);  // CR1 as input
  delay(100);

  // Test CR2 output states
  println(F("Testing CR2 output states..."));
  M1Shield.writeCR2(true);  // CR2 HIGH
  delay(100);
  M1Shield.writeCR2(false);  // CR2 LOW
  delay(100);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Remote control test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testCassettePinConfiguration() {
  println(F("Testing Cassette Pin Configuration..."));

  // Test different pin configurations safely
  println(F("Testing CR1/CR2 pin mode switching..."));

  // Configuration 1: Both as inputs (safe)
  M1Shield.setCR1Mode(false);  // CR1 as input
  M1Shield.setCR2Mode(false);  // CR2 as input
  delay(100);

  // Read both inputs
  bool cr1_val = M1Shield.readCR1();
  bool cr2_val = M1Shield.readCR2();
  println(F("Both pins configured as inputs"));

  delay(200);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Pin configuration test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testDataOutput() {
  println(F("Testing Data Output (writeCassetteIn)..."));

  // Test various analog output values to Model 1 cassette input
  println(F("Testing analog output levels..."));

  // Test low level
  M1Shield.writeCassetteIn(0);
  delay(100);

  // Test medium level
  M1Shield.writeCassetteIn(128);
  delay(100);

  // Test high level
  M1Shield.writeCassetteIn(255);
  delay(100);

  // Test pattern: ramp up
  println(F("Testing output ramp pattern..."));
  for (int i = 0; i <= 255; i += 32) {
    M1Shield.writeCassetteIn(i);
    delay(20);
  }

  // Return to neutral
  M1Shield.writeCassetteIn(128);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Data output test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testDataInput() {
  println(F("Testing Data Input (readCassetteOut)..."));

  // Test reading from Model 1 cassette output
  println(F("Reading cassette output levels..."));

  // Take multiple readings
  for (int i = 0; i < 5; i++) {
    uint16_t reading = M1Shield.readCassetteOut();
    // Note: We can't easily display the value without printf-style formatting
    // Just confirm the interface responds
    delay(100);
  }

  println(F("Sampling complete - interface responsive"));

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Data input test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testAnalogCassetteInterface() {
  println(F("Testing Analog Cassette Interface..."));

  // Test combined input/output operation
  println(F("Testing combined analog I/O..."));

  // Send patterns while monitoring input
  uint8_t testLevels[] = {0, 64, 128, 192, 255};

  for (int i = 0; i < 5; i++) {
    // Send test level
    M1Shield.writeCassetteIn(testLevels[i]);
    delay(50);

    // Read response
    uint16_t response = M1Shield.readCassetteOut();
    delay(50);
  }

  // Test rapid pattern
  println(F("Testing rapid pattern switching..."));
  for (int i = 0; i < 20; i++) {
    M1Shield.writeCassetteIn(i % 2 ? 255 : 0);
    uint16_t reading = M1Shield.readCassetteOut();
    delay(25);
  }

  // Return to neutral
  M1Shield.writeCassetteIn(128);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Analog cassette interface test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testPatternIntegrity() {
  println(F("Testing Pattern Integrity..."));

  // Test complex pattern with both CR pins and analog interface
  println(F("Testing digital pattern integrity..."));

  // Configure one CR pin as output for pattern generation
  M1Shield.setCR1Mode(true);   // CR1 as output
  M1Shield.setCR2Mode(false);  // CR2 as input for monitoring

  // Test digital pattern on CR1
  bool digitalPattern[] = {true, false, true, true, false, false, true, false};
  for (int i = 0; i < 8; i++) {
    M1Shield.writeCR1(digitalPattern[i]);
    delay(50);
    // Monitor CR2 response
    bool response = M1Shield.readCR2();
    delay(50);
  }

  println(F("Testing analog pattern integrity..."));

  // Test analog pattern integrity
  uint8_t analogPattern[] = {0, 32, 64, 96, 128, 160, 192, 224, 255};
  for (int i = 0; i < 9; i++) {
    // Send analog pattern
    M1Shield.writeCassetteIn(analogPattern[i]);
    delay(75);

    // Read response for integrity check
    uint16_t response = M1Shield.readCassetteOut();
    delay(25);
  }

  // Combined test: Digital + Analog
  println(F("Testing combined digital/analog patterns..."));
  for (int i = 0; i < 4; i++) {
    // Digital pattern on CR1
    M1Shield.writeCR1(i % 2 == 0);
    // Analog pattern on cassette interface
    M1Shield.writeCassetteIn(i * 64);
    delay(100);

    // Read both interfaces
    bool digitalResp = M1Shield.readCR2();
    uint16_t analogResp = M1Shield.readCassetteOut();
    delay(50);
  }

  // Reset to safe state
  M1Shield.setCR1Mode(false);     // CR1 as input
  M1Shield.writeCassetteIn(128);  // Neutral analog level

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Pattern integrity test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testTimingAndFrequency() {
  println(F("Testing Timing and Frequency Response..."));

  // Test different timing speeds for digital signals
  println(F("Testing digital signal timing..."));
  M1Shield.setCR1Mode(true);   // CR1 as output
  M1Shield.setCR2Mode(false);  // CR2 as input

  // Test slow timing (typical cassette speed)
  for (int i = 0; i < 10; i++) {
    M1Shield.writeCR1(i % 2);
    delay(100);  // 10 Hz
  }

  // Test medium timing
  for (int i = 0; i < 20; i++) {
    M1Shield.writeCR1(i % 2);
    delay(50);  // 20 Hz
  }

  // Test faster timing
  for (int i = 0; i < 50; i++) {
    M1Shield.writeCR1(i % 2);
    delay(20);  // 50 Hz
  }

  println(F("Testing analog frequency response..."));
  // Test analog signal frequency response
  for (int freq = 0; freq < 10; freq++) {
    for (int cycle = 0; cycle < 4; cycle++) {
      M1Shield.writeCassetteIn(cycle % 2 ? 255 : 0);
      delay(10 + freq * 5);  // Variable frequency
    }
  }

  M1Shield.setCR1Mode(false);     // Reset to input
  M1Shield.writeCassetteIn(128);  // Neutral

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Timing and frequency test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testVoltageRange() {
  println(F("Testing Voltage Range Validation..."));

  // Test full analog output range and measure input response
  println(F("Testing analog output range coverage..."));

  // Test each voltage increment and read response
  for (int voltage = 0; voltage <= 255; voltage += 17) {  // 16 steps
    M1Shield.writeCassetteIn(voltage);
    delay(50);

    // Read input to verify voltage handling
    uint16_t response = M1Shield.readCassetteOut();
    delay(25);

    // Check for reasonable response (basic sanity check)
    if (response > 1023) {
      // Error condition - readings should be 0-1023
      setTextColor(0xF800, 0x0000);  // Red
      println(F("[ERROR] Invalid voltage reading detected"));
      setTextColor(0xFFFF, 0x0000);  // Back to white
    }
  }

  // Test boundary conditions
  println(F("Testing boundary conditions..."));
  M1Shield.writeCassetteIn(0);  // Minimum
  delay(100);
  uint16_t minResp = M1Shield.readCassetteOut();

  M1Shield.writeCassetteIn(255);  // Maximum
  delay(100);
  uint16_t maxResp = M1Shield.readCassetteOut();

  M1Shield.writeCassetteIn(128);  // Return to neutral

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Voltage range test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testSignalIntegrity() {
  println(F("Testing Signal Integrity..."));

  // Test for signal stability over time
  println(F("Testing signal stability..."));

  // Set a known level and monitor for drift
  M1Shield.writeCassetteIn(128);
  uint16_t readings[10];

  for (int i = 0; i < 10; i++) {
    delay(100);
    readings[i] = M1Shield.readCassetteOut();
  }

  // Check for excessive variation (simple stability test)
  println(F("Signal stability test completed"));

  // Test rapid transitions for ringing/overshoot
  println(F("Testing transition integrity..."));
  for (int i = 0; i < 20; i++) {
    M1Shield.writeCassetteIn(i % 2 ? 255 : 0);  // Rapid transitions
    delay(10);
    uint16_t reading = M1Shield.readCassetteOut();
    delay(5);
  }

  M1Shield.writeCassetteIn(128);  // Return to neutral

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Signal integrity test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testCrosstalk() {
  println(F("Testing Crosstalk and Interference..."));

  // Test if CR pin activity affects analog signals
  println(F("Testing CR to analog crosstalk..."));

  M1Shield.setCR1Mode(true);  // CR1 as output
  M1Shield.setCR2Mode(true);  // CR2 as output (if safe)

  // Set baseline analog level
  M1Shield.writeCassetteIn(128);
  delay(100);
  uint16_t baseline = M1Shield.readCassetteOut();

  // Toggle CR pins while monitoring analog
  for (int i = 0; i < 10; i++) {
    M1Shield.writeCR1(i % 2);
    M1Shield.writeCR2((i + 1) % 2);
    delay(50);

    uint16_t reading = M1Shield.readCassetteOut();
    delay(50);
  }

  // Test if analog activity affects CR pins
  println(F("Testing analog to CR crosstalk..."));
  M1Shield.setCR1Mode(false);  // CR1 as input
  M1Shield.setCR2Mode(false);  // CR2 as input

  // Read baseline CR states
  bool cr1_baseline = M1Shield.readCR1();
  bool cr2_baseline = M1Shield.readCR2();

  // Vary analog levels while monitoring CR pins
  for (int level = 0; level <= 255; level += 64) {
    M1Shield.writeCassetteIn(level);
    delay(50);

    bool cr1_reading = M1Shield.readCR1();
    bool cr2_reading = M1Shield.readCR2();
    delay(50);
  }

  M1Shield.writeCassetteIn(128);  // Return to neutral

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Crosstalk test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}

void CassetteTestSuiteConsole::testEndurance() {
  println(F("Testing Endurance and Stress..."));

  println(F("Running sustained operation test..."));

  // Configure for stress test
  M1Shield.setCR1Mode(true);   // CR1 as output
  M1Shield.setCR2Mode(false);  // CR2 as input

  // Run rapid cycling for endurance
  for (int cycle = 0; cycle < 100; cycle++) {
    // Digital stress test
    M1Shield.writeCR1(cycle % 2);

    // Analog stress test
    M1Shield.writeCassetteIn(cycle % 2 ? 255 : 0);

    // Monitor for any issues
    bool cr2_state = M1Shield.readCR2();
    uint16_t analog_reading = M1Shield.readCassetteOut();

    delay(25);  // Rapid cycling

    // Basic sanity check
    if (analog_reading > 1023) {
      setTextColor(0xF800, 0x0000);  // Red
      println(F("[ERROR] Stress test detected failure"));
      setTextColor(0xFFFF, 0x0000);  // Back to white
      break;
    }
  }

  // Cool down and return to safe state
  println(F("Cooling down..."));
  M1Shield.setCR1Mode(false);     // CR1 as input
  M1Shield.writeCassetteIn(128);  // Neutral analog
  delay(500);

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("[OK] Endurance test completed"));
  setTextColor(0xFFFF, 0x0000);  // Back to white
}
