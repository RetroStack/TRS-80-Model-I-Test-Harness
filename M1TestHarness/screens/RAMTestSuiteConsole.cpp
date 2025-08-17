#include "./RAMTestSuiteConsole.h"

#include <Arduino.h>
#include <M1Shield.h>
#include <Model1.h>

#include "../globals.h"

#define INIT_TEST_RESULT TestResult result = {}
#define UPDATE_ERRORS(diff)           \
  if (diff != 0) {                    \
    result.totalErrors++;             \
    for (uint8_t b = 0; b < 8; b++) { \
      if (diff & (1 << b))            \
        result.bitErrors[b]++;        \
    }                                 \
  }

RAMTestSuiteConsole::RAMTestSuiteConsole() : ConsoleScreen() {
  Globals.logger.infoF(F("RAM Test Suite base class initialized"));
}

void RAMTestSuiteConsole::runSpecializedTest(uint16_t start, uint16_t length,
                                             const char *const icRefs[]) {
  runAndEvaluate(start, length, icRefs);
}

TestResult RAMTestSuiteConsole::runRepeatedWriteTest(uint16_t start, uint16_t length,
                                                     bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Repeated Write Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    for (uint16_t j = 0; j < 5; j++) {
      Model1.writeMemory(start + i, 0x55);
    }
  }
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x55;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runRepeatedReadTest(uint16_t start, uint16_t length,
                                                    bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Repeated Read Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x55);
  }
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    for (uint16_t j = 0; j < 5; j++) {
      data = Model1.readMemory(start + i);
    }
    uint8_t diff = data ^ 0x55;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runCheckerboardTest(uint16_t start, uint16_t length,
                                                    bool toggleStart) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Checkerboard Test"));
  setTextColor(0xFFFF, 0x0000);  // White
  if (toggleStart) {
    print(F("(normal)"));
  } else {
    print(F("(inverted)"));
  }

  print(F("."));
  bool toggle = toggleStart;
  for (uint16_t i = 0; i < length; i++) {
    if (toggle) {
      Model1.writeMemory(start + i, 0x55);
    } else {
      Model1.writeMemory(start + i, 0xAA);
    }
    toggle = !toggle;
  }
  print(F("."));
  toggle = toggleStart;
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    if (toggle) {
      uint8_t diff = data ^ 0x55;
      UPDATE_ERRORS(diff);
    } else {
      uint8_t diff = data ^ 0xAA;
      UPDATE_ERRORS(diff);
    }
    toggle = !toggle;
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runWalkingOnesTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Walking Ones Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  for (uint8_t bit = 0; bit < 8; bit++) {
    uint8_t pattern = 1 << bit;
    print(F("."));

    // Write
    for (uint16_t i = 0; i < length; i++) {
      Model1.writeMemory(start + i, pattern);
    }

    // Read
    for (uint16_t i = 0; i < length; i++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
    }
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runWalkingZerosTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Walking Zeros Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  for (uint8_t bit = 0; bit < 8; bit++) {
    uint8_t pattern = ~(1 << bit);
    print(F("."));

    // Write
    for (uint16_t i = 0; i < length; i++) {
      Model1.writeMemory(start + i, pattern);
    }

    // Read
    for (uint16_t i = 0; i < length; i++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
    }
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runMarchCTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("March C- Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  print(F("."));
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  print(F("."));
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runMovingInversionTest(uint16_t start, uint16_t length,
                                                       uint8_t pattern) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Moving Inversion Test (0x"));
  print(pattern, HEX);
  print(F(")"));
  setTextColor(0xFFFF, 0x0000);  // White

  // First pattern: 0x00
  uint8_t antipattern = ~pattern;

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, antipattern);
  }

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ antipattern;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, pattern);
  }

  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runRetentionTest(uint16_t start, uint16_t length, uint8_t pattern,
                                                 uint32_t delayMs, uint8_t repeatDelay) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Retention Test (0x"));
  print(pattern, HEX);
  print(F(")"));
  setTextColor(0xFFFF, 0x0000);  // White

  // Fill with pattern
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  for (uint16_t i = 0; i < repeatDelay; i++) {
    print(F("."));
    delay(delayMs);
  }
  // Verify
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ pattern;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runMarchSSTest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("March SS Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  // Phase 1: Up write 0
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 2: Up read 0, write 1
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 3: Down read 1, write 0
  print(F("."));
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 4: Down read 0, write 1
  print(F("."));
  for (int16_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 5: Up read 1, write 0
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 6: Up read 0
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runMarchLATest(uint16_t start, uint16_t length) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("March LA Test"));
  setTextColor(0xFFFF, 0x0000);  // White

  // Phase 1: Up write 0
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 2: Up read 0, write 1
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0xFF);
  }

  // Phase 3: Down read 1, write 0
  print(F("."));
  for (int32_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0xFF;
    UPDATE_ERRORS(diff);
    Model1.writeMemory(start + i, 0x00);
  }

  // Phase 4: Down read 0
  print(F("."));
  for (int32_t i = length - 1; i >= 0; i--) {
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ 0x00;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runReadDestructiveTest(uint16_t start, uint16_t length,
                                                       uint8_t pattern, uint8_t numReads) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Read Destructive Fault Test (pattern 0x"));
  print(pattern, HEX);
  print(F(", reads: "));
  print(numReads);
  print(F(")"));
  setTextColor(0xFFFF, 0x0000);  // White

  // 1. Fill memory with pattern
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    Model1.writeMemory(start + i, pattern);
  }

  // 2. Repeatedly read the same cell without re-writing
  for (uint16_t i = 0; i < length; i++) {
    for (uint8_t r = 0; r < numReads; r++) {
      data = Model1.readMemory(start + i);
      uint8_t diff = data ^ pattern;
      UPDATE_ERRORS(diff);
      if (diff != 0) {
        break;
      }
    }
  }
  println();

  return result;
}

TestResult RAMTestSuiteConsole::runAddressUniquenessTest(uint16_t start, uint16_t length,
                                                         uint8_t pattern) {
  uint8_t data;
  INIT_TEST_RESULT;

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Address Uniqueness Test (XOR pattern 0x"));
  print(pattern, HEX);
  print(F(")"));
  setTextColor(0xFFFF, 0x0000);  // White

  // Phase 1: Write unique XOR pattern
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    uint8_t value = ((uint8_t)(i & 0xFF)) ^ pattern;
    Model1.writeMemory(start + i, value);
  }

  // Phase 2: Verify
  print(F("."));
  for (uint16_t i = 0; i < length; i++) {
    uint8_t expected = ((uint8_t)(i & 0xFF)) ^ pattern;
    data = Model1.readMemory(start + i);
    uint8_t diff = data ^ expected;
    UPDATE_ERRORS(diff);
  }
  println();

  return result;
}

void RAMTestSuiteConsole::runAndEvaluate(uint16_t start, uint16_t length,
                                         const char *const icRefs[]) {
  Model1.activateTestSignal();
  M1Shield.setLEDColor(COLOR_BLUE);  // Initialize test suite
  setTextColor(0xFFFF, 0x0000);      // White

  println(F("=== START MEMORY TEST SUITE ==="));

  // Initialize aggregate totals - no intermediate storage needed
  uint32_t totalBitErrors[8] = {};
  uint32_t totalErrorsOverall = 0;

  // Run comprehensive test suite and accumulate results directly
  TestResult tempResult;

  setProgressValue(5);
  M1Shield.setLEDColor(COLOR_CYAN);  // Basic write tests
  tempResult = runRepeatedWriteTest(start, length, true);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(10);
  tempResult = runRepeatedWriteTest(start, length, false);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(15);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Read tests
  tempResult = runRepeatedReadTest(start, length, true);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(20);
  tempResult = runRepeatedReadTest(start, length, false);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(25);
  M1Shield.setLEDColor(COLOR_CYAN);  // Pattern tests
  tempResult = runCheckerboardTest(start, length, true);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(30);
  tempResult = runCheckerboardTest(start, length, false);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(35);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Walking bit tests
  tempResult = runWalkingOnesTest(start, length);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(40);
  tempResult = runWalkingZerosTest(start, length);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(45);
  M1Shield.setLEDColor(COLOR_CYAN);  // March tests (algorithmic)
  tempResult = runMarchCTest(start, length);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(50);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Moving inversion tests
  tempResult = runMovingInversionTest(start, length, 0x00);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(55);
  tempResult = runMovingInversionTest(start, length, 0x55);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(60);
  tempResult = runMovingInversionTest(start, length, random(0, 255));
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(65);
  M1Shield.setLEDColor(COLOR_CYAN);  // Advanced march tests
  tempResult = runMarchSSTest(start, length);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(70);
  tempResult = runMarchLATest(start, length);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(75);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Destructive/stress tests
  tempResult = runReadDestructiveTest(start, length, 0xAA, 5);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(80);
  tempResult = runReadDestructiveTest(start, length, 0x55, 5);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(85);
  M1Shield.setLEDColor(COLOR_CYAN);  // Address uniqueness tests
  tempResult = runAddressUniquenessTest(start, length, 0x55);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(90);
  tempResult = runAddressUniquenessTest(start, length, 0xAA);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(95);
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Final retention test
  tempResult = runRetentionTest(start, length, 0xFF, 1000, 5);
  totalErrorsOverall += tempResult.totalErrors;
  for (uint8_t b = 0; b < 8; b++)
    totalBitErrors[b] += tempResult.bitErrors[b];

  setProgressValue(100);

  cls();
  println(F("--- Summary ---"));

  // Print per-bit totals using PROGMEM strings
  for (uint8_t b = 0; b < 8; b++) {
    setTextColor(0xFFFF, 0x0000);  // White
    print(F("Bit "));
    print(b);
    print(F(" ("));
    print(icRefs[b]);
    print(F("): "));
    if (totalBitErrors[b] == 0) {
      setTextColor(0x07E0, 0x0000);  // Green
    } else {
      setTextColor(0xF800, 0x0000);  // Red
    }
    println(totalBitErrors[b]);
  }
  setTextColor(0xFFFF, 0x0000);  // White

  print(F("Total Errors: "));
  if (totalErrorsOverall == 0) {
    setTextColor(0x07E0, 0x0000);  // Green
  } else {
    setTextColor(0xF800, 0x0000);  // Red
  }
  println(totalErrorsOverall);
  setTextColor(0xFFFF, 0x0000);  // White

  Model1.deactivateTestSignal();

  // Final LED status indication based on test results
  if (totalErrorsOverall == 0) {
    // All tests passed
    M1Shield.setLEDColor(COLOR_GREEN);
  } else {
    // Errors found
    M1Shield.setLEDColor(COLOR_RED);
  }

  // Keep it visible for 2 seconds
  delay(2000);
  M1Shield.setLEDColor(COLOR_OFF);
}
