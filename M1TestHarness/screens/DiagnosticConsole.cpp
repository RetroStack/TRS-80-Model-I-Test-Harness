#include "./DiagnosticConsole.h"

#include <Arduino.h>
#include <M1Shield.h>
#include <Model1.h>
#include <Model1LowLevel.h>

#include "../globals.h"
#include "./MainMenu.h"
#include "Model1LowLevel.h"

constexpr uint16_t SETTLE_DELAY = 10;
constexpr uint16_t CONFIRM_DELAY = 1;
constexpr uint16_t CONFIRM_LOOPS = 200;

DiagnosticConsole::DiagnosticConsole() : ConsoleScreen() {
  setTitleF(F("Diagnostic"));
  setConsoleBackground(0x0000);
  setTextColor(0xFFFF, 0x0000);
  setTextSize(2);

  clearButtonItems();

  Globals.logger.infoF(F("Diagnostic Screen initialized"));
}

bool DiagnosticConsole::open() {
  // Call parent implementation
  bool result = ConsoleScreen::open();

  // Stop Model1 to allow low-level access
  Model1.end();

  return result;
}

void DiagnosticConsole::close() {
  // Turn off any indicator for now
  M1Shield.setLEDColor(COLOR_OFF);

  // Restart Model1 when leaving diagnostic screen
  Model1.begin();

  // Call parent implementation
  ConsoleScreen::close();
}

// --- Bus verification result structs ---
struct DataBusVerificationResult {
  uint8_t stuckHigh;
  uint8_t stuckLow;
  bool hasIssues;
};
struct AddressBusVerificationResult {
  uint16_t stuckHigh;
  uint16_t stuckLow;
  bool hasIssues;
};

struct ControlBusVerificationResult {
  uint16_t stuckHigh;  // Control signals stuck high
  uint16_t stuckLow;   // Control signals stuck low
  bool hasIssues;
};

struct UnifiedCrosstalkResult {
  uint64_t crosstalkSourceMask;       // 64-bit mask showing which signals cause crosstalk
  uint64_t crosstalkDestinationMask;  // 64-bit mask showing which signals are affected by crosstalk
  bool hasIssues;

  // Signal mapping for interpretation:
  // Bits 0-7:   Data bus (D0-D7)
  // Bits 8-23:  Address bus (A0-A15)
  // Bits 24-35: Control signals (RAS, MUX, CAS, RD, WR, IN, OUT, SYS_RES, INT_ACK, INT, TEST, WAIT)
  // Bits 36-63: Reserved for future expansion
};

static DataBusVerificationResult verifyDataBus() {
  uint8_t stuckHigh = 0;
  uint8_t stuckLow = 0;
  bool issuesFound = false;
  uint8_t data = 0;

  // Note: TEST signal (BUSREQ) should already be activated at top level

  // Stuck-high test
  Model1LowLevel::configWriteDataBus(0x00);
  Model1LowLevel::writeDataBus(0x00);
  delay(SETTLE_DELAY);
  data = Model1LowLevel::readDataBus();
  for (uint8_t i = 0; i < 8; i++) {
    if ((data & (1 << i)) > 0) {
      uint16_t bitIssue = 0;
      for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
        delay(CONFIRM_DELAY);
        data = Model1LowLevel::readDataBus();
        if ((data & (1 << i)) > 0)
          bitIssue++;
      }
      if (bitIssue >= CONFIRM_LOOPS) {
        issuesFound = true;
        bitSet(stuckHigh, i);
      }
    }
  }

  // Stuck-low test
  Model1LowLevel::writeDataBus(0xFF);
  delay(SETTLE_DELAY);
  data = Model1LowLevel::readDataBus();
  for (uint8_t i = 0; i < 8; i++) {
    if ((data & (1 << i)) == 0) {
      issuesFound = true;
      bitSet(stuckLow, i);
    }
  }

  // Clean up
  Model1LowLevel::configWriteDataBus(0x00);
  Model1LowLevel::writeDataBus(0x00);
  delay(SETTLE_DELAY);

  DataBusVerificationResult result;
  result.stuckLow = stuckLow;
  result.stuckHigh = stuckHigh;
  result.hasIssues = issuesFound;
  return result;
}

static AddressBusVerificationResult verifyAddressBus() {
  uint16_t stuckHigh = 0;
  uint16_t stuckLow = 0;
  bool issuesFound = false;
  uint16_t data = 0;

  // Note: TEST signal (BUSREQ) should already be activated at top level

  // Stuck-high test
  Model1LowLevel::configWriteAddressBus(0x0000);
  Model1LowLevel::writeAddressBus(0x0000);
  delay(SETTLE_DELAY);
  data = Model1LowLevel::readAddressBus();
  for (uint16_t i = 0; i < 16; i++) {
    if ((data & (1 << i)) > 0) {
      uint16_t bitIssue = 0;
      for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
        delay(CONFIRM_DELAY);
        data = Model1LowLevel::readAddressBus();
        if ((data & (1 << i)) > 0)
          bitIssue++;
      }
      if (bitIssue >= CONFIRM_LOOPS) {
        issuesFound = true;
        bitSet(stuckHigh, i);
      }
    }
  }

  // Stuck-low test
  Model1LowLevel::writeAddressBus(0xFFFF);
  delay(SETTLE_DELAY);
  data = Model1LowLevel::readAddressBus();
  for (uint16_t i = 0; i < 16; i++) {
    if ((data & (1 << i)) == 0) {
      issuesFound = true;
      bitSet(stuckLow, i);
    }
  }

  // Clean up
  Model1LowLevel::configWriteAddressBus(0x0000);
  Model1LowLevel::writeAddressBus(0x0000);
  delay(SETTLE_DELAY);

  AddressBusVerificationResult result;
  result.stuckLow = stuckLow;
  result.stuckHigh = stuckHigh;
  result.hasIssues = issuesFound;
  return result;
}

static UnifiedCrosstalkResult verifyUnifiedCrosstalk() {
  UnifiedCrosstalkResult result = {0};
  bool issuesFound = false;

  // Signal bit positions in the 64-bit unified space
  const uint8_t DATA_BASE = 0;   // Data bus: bits 0-7
  const uint8_t ADDR_BASE = 8;   // Address bus: bits 8-23
  const uint8_t CTRL_BASE = 24;  // Control signals: bits 24-35

  // Note: TEST signal (BUSREQ) should already be activated at top level

  // Helper function to read all signals into a 64-bit value
  auto readAllSignals = []() -> uint64_t {
    uint64_t signals = 0;

    // Read data bus (bits 0-7)
    uint8_t dataValue = Model1LowLevel::readDataBus();
    signals |= (uint64_t)dataValue;

    // Read address bus (bits 8-23)
    uint16_t addrValue = Model1LowLevel::readAddressBus();
    signals |= ((uint64_t)addrValue << ADDR_BASE);

    // Read control signals (bits 24-35)
    uint16_t controlValue = 0;
    if (Model1LowLevel::readRAS())
      bitSet(controlValue, 0);  // RAS
    if (Model1LowLevel::readMUX())
      bitSet(controlValue, 1);  // MUX
    if (Model1LowLevel::readCAS())
      bitSet(controlValue, 2);  // CAS
    if (Model1LowLevel::readRD())
      bitSet(controlValue, 3);  // RD
    if (Model1LowLevel::readWR())
      bitSet(controlValue, 4);  // WR
    if (Model1LowLevel::readIN())
      bitSet(controlValue, 5);  // IN
    if (Model1LowLevel::readOUT())
      bitSet(controlValue, 6);  // OUT
    if (Model1LowLevel::readSYS_RES())
      bitSet(controlValue, 7);  // SYS_RES (read-only)
    if (Model1LowLevel::readINT_ACK())
      bitSet(controlValue, 8);  // INT_ACK (read-only)
    if (Model1LowLevel::readINT())
      bitSet(controlValue, 9);  // INT (write-only, but can read when set to input)
    if (Model1LowLevel::readTEST())
      bitSet(controlValue, 10);  // TEST (we know this is active)
    if (Model1LowLevel::readWAIT())
      bitSet(controlValue, 11);  // WAIT (write-only, but can read when set to input)
    signals |= ((uint64_t)controlValue << CTRL_BASE);

    return signals;
  };

  // Helper function to set a signal by bit position
  auto setSignal = [](uint8_t bitPos, bool value) {
    if (bitPos < DATA_BASE + 8) {
      // Data bus signal (read/write)
      uint8_t dataBit = bitPos - DATA_BASE;
      uint8_t currentConfig = 0;
      bitSet(currentConfig, dataBit);
      Model1LowLevel::configWriteDataBus(currentConfig);
      Model1LowLevel::writeDataBus(value ? ~currentConfig
                                         : currentConfig);  // Active low for testing
    } else if (bitPos < ADDR_BASE + 16) {
      // Address bus signal (read/write)
      uint8_t addrBit = bitPos - ADDR_BASE;
      uint16_t currentConfig = 0;
      bitSet(currentConfig, addrBit);
      Model1LowLevel::configWriteAddressBus(currentConfig);
      Model1LowLevel::writeAddressBus(value ? ~currentConfig
                                            : currentConfig);  // Active low for testing
    } else if (bitPos < CTRL_BASE + 12) {
      // Control signals
      uint8_t ctrlBit = bitPos - CTRL_BASE;
      switch (ctrlBit) {
        case 0:  // RAS (active low, read/write)
          Model1LowLevel::configWriteRAS(OUTPUT);
          Model1LowLevel::writeRAS(value ? LOW : HIGH);
          break;
        case 1:  // MUX (active low, read/write)
          Model1LowLevel::configWriteMUX(OUTPUT);
          Model1LowLevel::writeMUX(value ? LOW : HIGH);
          break;
        case 2:  // CAS (active low, read/write)
          Model1LowLevel::configWriteCAS(OUTPUT);
          Model1LowLevel::writeCAS(value ? LOW : HIGH);
          break;
        case 3:  // RD (active low, read/write)
          Model1LowLevel::configWriteRD(OUTPUT);
          Model1LowLevel::writeRD(value ? LOW : HIGH);
          break;
        case 4:  // WR (active low, read/write)
          Model1LowLevel::configWriteWR(OUTPUT);
          Model1LowLevel::writeWR(value ? LOW : HIGH);
          break;
        case 5:  // IN (active low, read/write)
          Model1LowLevel::configWriteIN(OUTPUT);
          Model1LowLevel::writeIN(value ? LOW : HIGH);
          break;
        case 6:  // OUT (active low, read/write)
          Model1LowLevel::configWriteOUT(OUTPUT);
          Model1LowLevel::writeOUT(value ? LOW : HIGH);
          break;
        case 9:  // INT (active low, write-only, but can test as input)
          Model1LowLevel::configWriteINT(OUTPUT);
          Model1LowLevel::writeINT(value ? LOW : HIGH);
          break;
        case 11:  // WAIT (active low, write-only, but can test as input)
          Model1LowLevel::configWriteWAIT(OUTPUT);
          Model1LowLevel::writeWAIT(value ? LOW : HIGH);
          break;
          // Skip SYS_RES (7) and INT_ACK (8) as they are read-only
          // Skip TEST (10) as we need it active for testing
      }
    }
  };

  // Helper function to reset a signal
  auto resetSignal = [](uint8_t bitPos) {
    if (bitPos < DATA_BASE + 8) {
      // Data bus - reset to tristate
      Model1LowLevel::configWriteDataBus(0x00);
      Model1LowLevel::writeDataBus(0x00);
    } else if (bitPos < ADDR_BASE + 16) {
      // Address bus - reset to tristate
      Model1LowLevel::configWriteAddressBus(0x0000);
      Model1LowLevel::writeAddressBus(0x0000);
    } else if (bitPos < CTRL_BASE + 12) {
      // Control signals - reset to inactive state
      uint8_t ctrlBit = bitPos - CTRL_BASE;
      switch (ctrlBit) {
        case 0:  // RAS
          Model1LowLevel::configWriteRAS(INPUT);
          Model1LowLevel::writeRAS(LOW);
          break;
        case 1:  // MUX
          Model1LowLevel::configWriteMUX(INPUT);
          Model1LowLevel::writeMUX(LOW);
          break;
        case 2:  // CAS
          Model1LowLevel::configWriteCAS(INPUT);
          Model1LowLevel::writeCAS(LOW);
          break;
        case 3:  // RD
          Model1LowLevel::configWriteRD(INPUT);
          Model1LowLevel::writeRD(LOW);
          break;
        case 4:  // WR
          Model1LowLevel::configWriteWR(INPUT);
          Model1LowLevel::writeWR(LOW);
          break;
        case 5:  // IN
          Model1LowLevel::configWriteIN(INPUT);
          Model1LowLevel::writeIN(LOW);
          break;
        case 6:  // OUT
          Model1LowLevel::configWriteOUT(INPUT);
          Model1LowLevel::writeOUT(LOW);
          break;
        case 9:  // INT
          Model1LowLevel::configWriteINT(INPUT);
          Model1LowLevel::writeINT(LOW);
          break;
        case 11:  // WAIT
          Model1LowLevel::configWriteWAIT(INPUT);
          Model1LowLevel::writeWAIT(LOW);
          break;
      }
    }
  };

  // Initialize all signals to known state
  Model1LowLevel::configWriteDataBus(0x00);
  Model1LowLevel::writeDataBus(0x00);
  Model1LowLevel::configWriteAddressBus(0x0000);
  Model1LowLevel::writeAddressBus(0x0000);

  // Set control signals to inactive state (INPUT mode, no pull-ups)
  Model1LowLevel::configWriteRAS(INPUT);
  Model1LowLevel::writeRAS(LOW);
  Model1LowLevel::configWriteMUX(INPUT);
  Model1LowLevel::writeMUX(LOW);
  Model1LowLevel::configWriteCAS(INPUT);
  Model1LowLevel::writeCAS(LOW);
  Model1LowLevel::configWriteRD(INPUT);
  Model1LowLevel::writeRD(LOW);
  Model1LowLevel::configWriteWR(INPUT);
  Model1LowLevel::writeWR(LOW);
  Model1LowLevel::configWriteIN(INPUT);
  Model1LowLevel::writeIN(LOW);
  Model1LowLevel::configWriteOUT(INPUT);
  Model1LowLevel::writeOUT(LOW);
  Model1LowLevel::configWriteINT(INPUT);
  Model1LowLevel::writeINT(LOW);
  Model1LowLevel::configWriteWAIT(INPUT);
  Model1LowLevel::writeWAIT(LOW);

  delay(SETTLE_DELAY);

  uint64_t baselineSignals = readAllSignals();

  // Test each signal as a potential crosstalk source
  for (uint8_t sourcePos = 0; sourcePos < 36; sourcePos++) {
    // Skip signals that cannot be tested as sources
    uint8_t ctrlBit = sourcePos - CTRL_BASE;
    if (sourcePos >= CTRL_BASE) {
      // Skip read-only signals and TEST signal
      if (ctrlBit == 7 || ctrlBit == 8 || ctrlBit == 10)
        continue;  // SYS_RES, INT_ACK, TEST
    }

    // Set the source signal
    setSignal(sourcePos, true);
    delay(SETTLE_DELAY);

    // Read all signals and check for unexpected changes
    uint64_t currentSignals = readAllSignals();
    uint64_t expectedMask = (1ULL << sourcePos);
    uint64_t unexpectedChanges = (currentSignals ^ baselineSignals) & ~expectedMask;

    if (unexpectedChanges != 0) {
      // Confirm the crosstalk with multiple readings
      uint16_t confirmedIssues = 0;
      for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
        delay(CONFIRM_DELAY);
        uint64_t confirmSignals = readAllSignals();
        uint64_t confirmChanges = (confirmSignals ^ baselineSignals) & ~expectedMask;
        if (confirmChanges != 0)
          confirmedIssues++;
      }

      if (confirmedIssues >= (CONFIRM_LOOPS * 0.8)) {
        issuesFound = true;
        result.crosstalkSourceMask |= (1ULL << sourcePos);
        result.crosstalkDestinationMask |= unexpectedChanges;
      }
    }

    // Reset signal
    resetSignal(sourcePos);
    delay(SETTLE_DELAY);
  }

  // Clean up - reset all signals to safe state
  Model1LowLevel::configWriteDataBus(0x00);
  Model1LowLevel::writeDataBus(0x00);
  Model1LowLevel::configWriteAddressBus(0x0000);
  Model1LowLevel::writeAddressBus(0x0000);

  // Reset control signals to safe input state
  Model1LowLevel::configWriteRAS(INPUT);
  Model1LowLevel::writeRAS(LOW);
  Model1LowLevel::configWriteMUX(INPUT);
  Model1LowLevel::writeMUX(LOW);
  Model1LowLevel::configWriteCAS(INPUT);
  Model1LowLevel::writeCAS(LOW);
  Model1LowLevel::configWriteRD(INPUT);
  Model1LowLevel::writeRD(LOW);
  Model1LowLevel::configWriteWR(INPUT);
  Model1LowLevel::writeWR(LOW);
  Model1LowLevel::configWriteIN(INPUT);
  Model1LowLevel::writeIN(LOW);
  Model1LowLevel::configWriteOUT(INPUT);
  Model1LowLevel::writeOUT(LOW);
  Model1LowLevel::configWriteINT(INPUT);
  Model1LowLevel::writeINT(LOW);
  Model1LowLevel::configWriteWAIT(INPUT);
  Model1LowLevel::writeWAIT(LOW);

  delay(SETTLE_DELAY);

  result.hasIssues = issuesFound;
  return result;
}

static ControlBusVerificationResult verifyControlBus() {
  const int INDEX_RAS = 0;
  const int INDEX_MUX = 1;
  const int INDEX_CAS = 2;
  const int INDEX_RD = 3;
  const int INDEX_WR = 4;
  const int INDEX_IN = 5;
  const int INDEX_OUT = 6;
  const int INDEX_SYS_RES = 7;
  const int INDEX_INT_ACK = 8;
  const int INDEX_INT = 9;
  const int INDEX_TEST = 10;
  const int INDEX_WAIT = 11;

  uint16_t bitIssues[2] = {0};  // 0=stuck low, 1=stuck high
  bool issuesFound = false;

  // Note: TEST signal (BUSREQ) should already be activated at top level
  // Verify TEST signal is active as expected
  if (Model1LowLevel::readTEST() != LOW) {
    // TEST is not active as expected - this is a problem
    bitSet(bitIssues[1], INDEX_TEST);  // TEST not properly activated
    issuesFound = true;
  }

  // ========== Verify write-only output signals (INT, WAIT) ==========
  // Test by briefly setting them as inputs to check behavior

  // Test INT signal
  Model1LowLevel::writeINT(LOW);
  Model1LowLevel::configWriteINT(INPUT);
  delay(SETTLE_DELAY);
  if (Model1LowLevel::readINT() == LOW) {
    bitSet(bitIssues[0], INDEX_INT);  // INT stuck low
    issuesFound = true;
  }

  // Test WAIT signal
  Model1LowLevel::writeWAIT(LOW);
  Model1LowLevel::configWriteWAIT(INPUT);
  delay(SETTLE_DELAY);
  if (Model1LowLevel::readWAIT() == LOW) {
    bitSet(bitIssues[0], INDEX_WAIT);  // WAIT stuck low
    issuesFound = true;
  }

  // ========== Verify read-only input signals (SYS_RES, INT_ACK) ==========
  // These should normally be inactive (HIGH) - if they're stuck LOW, it's an issue
  if (Model1LowLevel::readSYS_RES() == LOW) {
    // Note: SYS_RES should normally be inactive, but if constantly active, it's unusual
    // We'll mark this as potentially problematic but it might be normal depending on CPU state
  }
  if (Model1LowLevel::readINT_ACK() == LOW) {
    // Similar to SYS_RES - normally inactive
  }

  // ========== Verify read/write control signals for stuck-high conditions ==========

  // Set all read/write control signals to input mode with no pull-ups
  Model1LowLevel::configWriteRAS(INPUT);
  Model1LowLevel::writeRAS(LOW);
  Model1LowLevel::configWriteMUX(INPUT);
  Model1LowLevel::writeMUX(LOW);
  Model1LowLevel::configWriteCAS(INPUT);
  Model1LowLevel::writeCAS(LOW);
  Model1LowLevel::configWriteRD(INPUT);
  Model1LowLevel::writeRD(LOW);
  Model1LowLevel::configWriteWR(INPUT);
  Model1LowLevel::writeWR(LOW);
  Model1LowLevel::configWriteIN(INPUT);
  Model1LowLevel::writeIN(LOW);
  Model1LowLevel::configWriteOUT(INPUT);
  Model1LowLevel::writeOUT(LOW);

  delay(SETTLE_DELAY);

  // Test for stuck-high conditions
  if (Model1LowLevel::readRAS() == HIGH) {
    uint16_t bitIssue = 0;
    for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
      delay(CONFIRM_DELAY);
      if (Model1LowLevel::readRAS() == HIGH)
        bitIssue++;
    }
    if (bitIssue >= CONFIRM_LOOPS) {
      issuesFound = true;
      bitSet(bitIssues[1], INDEX_RAS);  // RAS stuck high
    }
  }

  if (Model1LowLevel::readMUX() == HIGH) {
    uint16_t bitIssue = 0;
    for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
      delay(CONFIRM_DELAY);
      if (Model1LowLevel::readMUX() == HIGH)
        bitIssue++;
    }
    if (bitIssue >= CONFIRM_LOOPS) {
      issuesFound = true;
      bitSet(bitIssues[1], INDEX_MUX);  // MUX stuck high
    }
  }

  if (Model1LowLevel::readCAS() == HIGH) {
    uint16_t bitIssue = 0;
    for (uint16_t j = 0; j < CONFIRM_LOOPS; j++) {
      delay(CONFIRM_DELAY);
      if (Model1LowLevel::readCAS() == HIGH)
        bitIssue++;
    }
    if (bitIssue >= CONFIRM_LOOPS) {
      issuesFound = true;
      bitSet(bitIssues[1], INDEX_CAS);  // CAS stuck high
    }
  }

  // Test other read/write control signals similarly
  if (Model1LowLevel::readRD() == HIGH) {
    issuesFound = true;
    bitSet(bitIssues[1], INDEX_RD);  // RD stuck high
  }
  if (Model1LowLevel::readWR() == HIGH) {
    issuesFound = true;
    bitSet(bitIssues[1], INDEX_WR);  // WR stuck high
  }
  if (Model1LowLevel::readIN() == HIGH) {
    issuesFound = true;
    bitSet(bitIssues[1], INDEX_IN);  // IN stuck high
  }
  if (Model1LowLevel::readOUT() == HIGH) {
    issuesFound = true;
    bitSet(bitIssues[1], INDEX_OUT);  // OUT stuck high
  }

  // ========== Test for stuck-low conditions ==========
  // Set signals as pull-ups to test for stuck-low
  Model1LowLevel::writeRAS(HIGH);
  Model1LowLevel::writeMUX(HIGH);
  Model1LowLevel::writeCAS(HIGH);
  Model1LowLevel::writeRD(HIGH);
  Model1LowLevel::writeWR(HIGH);
  Model1LowLevel::writeIN(HIGH);
  Model1LowLevel::writeOUT(HIGH);

  delay(SETTLE_DELAY);

  // Check if any are stuck low
  if (Model1LowLevel::readRAS() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_RAS);  // RAS stuck low
  }
  if (Model1LowLevel::readMUX() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_MUX);  // MUX stuck low
  }
  if (Model1LowLevel::readCAS() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_CAS);  // CAS stuck low
  }
  if (Model1LowLevel::readRD() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_RD);  // RD stuck low
  }
  if (Model1LowLevel::readWR() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_WR);  // WR stuck low
  }
  if (Model1LowLevel::readIN() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_IN);  // IN stuck low
  }
  if (Model1LowLevel::readOUT() == LOW) {
    issuesFound = true;
    bitSet(bitIssues[0], INDEX_OUT);  // OUT stuck low
  }

  // ========== Clean up ==========
  // Reset all control signals to safe state
  Model1LowLevel::configWriteRAS(INPUT);
  Model1LowLevel::writeRAS(LOW);
  Model1LowLevel::configWriteMUX(INPUT);
  Model1LowLevel::writeMUX(LOW);
  Model1LowLevel::configWriteCAS(INPUT);
  Model1LowLevel::writeCAS(LOW);
  Model1LowLevel::configWriteRD(INPUT);
  Model1LowLevel::writeRD(LOW);
  Model1LowLevel::configWriteWR(INPUT);
  Model1LowLevel::writeWR(LOW);
  Model1LowLevel::configWriteIN(INPUT);
  Model1LowLevel::writeIN(LOW);
  Model1LowLevel::configWriteOUT(INPUT);
  Model1LowLevel::writeOUT(LOW);
  Model1LowLevel::configWriteINT(INPUT);
  Model1LowLevel::writeINT(LOW);
  Model1LowLevel::configWriteWAIT(INPUT);
  Model1LowLevel::writeWAIT(LOW);

  delay(SETTLE_DELAY);

  ControlBusVerificationResult result;
  result.stuckLow = bitIssues[0];
  result.stuckHigh = bitIssues[1];
  result.hasIssues = issuesFound;
  return result;
}

void DiagnosticConsole::_executeOnce() {
  cls();
  setTextSize(1);
  setTextColor(0xFFFF, 0x0000);  // White
  println(F("Running bus verification..."));
  println(F("This may take a moment..."));
  println();

  setProgressValue(0);
  M1Shield.setLEDColor(COLOR_BLUE);  // Indicate test starting

  // ========== COMPREHENSIVE TEST SIGNAL VERIFICATION ==========
  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Checking TEST signal state..."));

  // First, check if TEST signal is already active (it shouldn't be)
  Model1LowLevel::configWriteTEST(INPUT);
  Model1LowLevel::writeTEST(LOW);  // No pull-up
  delay(SETTLE_DELAY);

  if (Model1LowLevel::readTEST() == LOW) {
    M1Shield.setLEDColor(COLOR_RED);  // Critical failure
    setTextColor(0xF800, 0x0000);     // Red
    println(F(" FAILED"));
    println(F("ERROR: TEST signal already"));
    println(F("active! Cannot proceed with"));
    println(F("bus testing. The TEST signal"));
    println(F("should be inactive (HIGH) when"));
    println(F("not in use. This indicates a"));
    println(F("hardware fault or another"));
    println(F("process controlling the bus."));
    println();
    setTextColor(0xFFFF, 0x0000);  // White
    println(F("Possible causes:"));
    println(F("- Short circuit on TEST line"));
    println(F("- Z80 BUSREQ stuck active"));
    println(F("- Hardware fault"));
    println(F("- Another test in progress"));
    println();
    setTextColor(0xF800, 0x0000);  // Red
    println(F("*** SYSTEM HALTED ***"));
    println(F("Fix hardware before retrying"));
    println();
    setTextColor(0x07E0, 0x0000);  // Yellow
    println(F("Press RESET to restart"));

    // Block progression - endless loop with pulsing red LED
    while (true) {
      M1Shield.setLEDColor(COLOR_RED);
      delay(500);
      M1Shield.setLEDColor(COLOR_OFF);
      delay(500);
    }
  }
  println(F(" OK (inactive)"));
  setProgressValue(2);

  // Now try to activate TEST signal
  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Activating TEST signal..."));
  Model1LowLevel::configWriteTEST(OUTPUT);
  Model1LowLevel::writeTEST(LOW);  // Activate TEST signal (active low - BUSREQ)
  delay(SETTLE_DELAY);

  // Verify TEST signal activation with multiple confirmations
  bool testActivated = false;
  uint16_t activationConfirms = 0;
  for (uint16_t i = 0; i < CONFIRM_LOOPS; i++) {
    delay(CONFIRM_DELAY);
    if (Model1LowLevel::readTEST() == LOW) {
      activationConfirms++;
    }
  }

  if (activationConfirms >= (CONFIRM_LOOPS * 0.9)) {
    testActivated = true;
  }

  if (!testActivated) {
    M1Shield.setLEDColor(COLOR_RED);  // Critical failure
    setTextColor(0xF800, 0x0000);     // Red
    println(F(" FAILED"));
    println(F("ERROR: Cannot activate TEST"));
    println(F("signal (BUSREQ). Bus access"));
    println(F("not available. TEST signal"));
    println(F("is not responding to control"));
    println(F("commands."));
    println();
    setTextColor(0xFFFF, 0x0000);  // White
    println(F("Possible causes:"));
    println(F("- Disconnected TEST line"));
    println(F("- Z80 BUSREQ not connected"));
    println(F("- GPIO pin fault"));
    println(F("- Hardware damage"));
    println();
    setTextColor(0xF800, 0x0000);  // Red
    println(F("*** SYSTEM HALTED ***"));
    println(F("Fix hardware before retrying"));
    println();
    setTextColor(0x07E0, 0x0000);  // Yellow
    println(F("Press RESET to restart"));

    // Block progression - endless loop with pulsing red LED
    while (true) {
      M1Shield.setLEDColor(COLOR_RED);
      delay(500);
      M1Shield.setLEDColor(COLOR_OFF);
      delay(500);
    }
  }
  println(F(" OK (active)"));
  setProgressValue(5);
  M1Shield.setLEDColor(COLOR_CYAN);  // Test in progress

  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Testing data bus..."));
  setProgressValue(10);                 // Starting data bus test
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Switch to magenta for data bus
  auto dataResult = verifyDataBus();
  println(F(" done"));
  setProgressValue(25);

  print(F("Testing address bus..."));
  setProgressValue(30);              // Starting address bus test
  M1Shield.setLEDColor(COLOR_CYAN);  // Switch to cyan for address bus
  auto addrResult = verifyAddressBus();
  println(F(" done"));
  setProgressValue(45);

  print(F("Testing control signals..."));
  setProgressValue(50);                 // Starting control signal test
  M1Shield.setLEDColor(COLOR_MAGENTA);  // Switch to magenta for control signals
  auto controlResult = verifyControlBus();
  println(F(" done"));
  setProgressValue(65);

  print(F("Testing unified crosstalk..."));
  setProgressValue(70);              // Starting unified crosstalk test
  M1Shield.setLEDColor(COLOR_CYAN);  // Switch to cyan for crosstalk
  auto crosstalkResult = verifyUnifiedCrosstalk();
  println(F(" done"));
  setProgressValue(85);
  println();

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("=== RESULTS ==="));
  println();
  setProgressValue(90);  // All tests complete, starting results

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("Data Bus Test:"));
  setTextColor(0xFFFF, 0x0000);
  if (dataResult.hasIssues) {
    if (dataResult.stuckHigh) {
      print(F("  Stuck High bits: D"));
      for (uint8_t i = 0; i < 8; i++) {
        if (dataResult.stuckHigh & (1 << i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
    if (dataResult.stuckLow) {
      print(F("  Stuck Low bits: D"));
      for (uint8_t i = 0; i < 8; i++) {
        if (dataResult.stuckLow & (1 << i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
  } else {
    println(F("  PASS"));
  }

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("Address Bus Test:"));
  setTextColor(0xFFFF, 0x0000);
  if (addrResult.hasIssues) {
    if (addrResult.stuckHigh) {
      print(F("  Stuck High bits: A"));
      for (uint16_t i = 0; i < 16; i++) {
        if (addrResult.stuckHigh & (1 << i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
    if (addrResult.stuckLow) {
      print(F("  Stuck Low bits: A"));
      for (uint16_t i = 0; i < 16; i++) {
        if (addrResult.stuckLow & (1 << i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
  } else {
    println(F("  PASS"));
  }

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("Control Bus Verification:"));
  setTextColor(0xFFFF, 0x0000);
  if (controlResult.hasIssues) {
    if (controlResult.stuckLow) {
      print(F("  Stuck LOW bits: "));
      for (int i = 0; i < 12; i++) {
        if (bitRead(controlResult.stuckLow, i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
    if (controlResult.stuckHigh) {
      print(F("  Stuck HIGH bits: "));
      for (int i = 0; i < 12; i++) {
        if (bitRead(controlResult.stuckHigh, i)) {
          print(i);
          print(F(" "));
        }
      }
      println();
    }
  } else {
    println(F("  PASS"));
  }

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("Unified Crosstalk Test:"));
  setTextColor(0xFFFF, 0x0000);
  if (crosstalkResult.hasIssues) {
    // Helper function to get signal name
    auto getSignalName = [](uint8_t bitPos) -> String {
      if (bitPos < 8)
        return "D" + String(bitPos);  // Data bus
      else if (bitPos < 24)
        return "A" + String(bitPos - 8);  // Address bus
      else if (bitPos < 36) {             // Control signals
        const char* names[] = {"RAS", "MUX",     "CAS",     "RD",  "WR",   "IN",
                               "OUT", "SYS_RES", "INT_ACK", "INT", "TEST", "WAIT"};
        uint8_t ctrlIndex = bitPos - 24;
        if (ctrlIndex < 12)
          return String(names[ctrlIndex]);
      }
      return "UNK" + String(bitPos);
    };

    print(F("  Crosstalk source signals: "));
    bool firstSource = true;
    for (uint8_t i = 0; i < 36; i++) {
      if (crosstalkResult.crosstalkSourceMask & (1ULL << i)) {
        if (!firstSource)
          print(F(", "));
        print(getSignalName(i));
        firstSource = false;
      }
    }
    if (firstSource)
      print(F("None"));
    println();

    print(F("  Affected signals: "));
    bool firstDest = true;
    for (uint8_t i = 0; i < 36; i++) {
      if (crosstalkResult.crosstalkDestinationMask & (1ULL << i)) {
        if (!firstDest)
          print(F(", "));
        print(getSignalName(i));
        firstDest = false;
      }
    }
    if (firstDest)
      print(F("None"));
    println();
  } else {
    println(F("  PASS"));
  }

  // Overall summary
  println();
  setProgressValue(95);  // Final results processing
  bool overallPass = !dataResult.hasIssues && !addrResult.hasIssues && !crosstalkResult.hasIssues &&
                     !controlResult.hasIssues;

  if (overallPass) {
    M1Shield.setLEDColor(COLOR_GREEN);  // Success indicator
    setTextColor(0x07E0, 0x0000);       // Green
    println(F("=== OVERALL: PASS ==="));
    setTextColor(0xFFFF, 0x0000);
    println(F("All bus tests completed"));
    println(F("successfully. Hardware"));
    println(F("appears to be functioning"));
    println(F("correctly."));
  } else {
    M1Shield.setLEDColor(COLOR_YELLOW);  // Warning indicator for issues
    setTextColor(0xF800, 0x0000);        // Red
    println(F("=== OVERALL: ISSUES FOUND ==="));
    setTextColor(0xFFFF, 0x0000);
    println(F("One or more tests failed."));
    println(F("Check connections and"));
    println(F("hardware for problems."));
  }
  println();

  setTextColor(0x07E0, 0x0000);  // Yellow
  println(F("Press any key to move to"));
  println(F("the next screen."));

  // ========== COMPREHENSIVE TEST SIGNAL DEACTIVATION ==========
  setTextColor(0x07FF, 0x0000);  // Cyan
  print(F("Deactivating TEST signal..."));

  Model1LowLevel::configWriteTEST(INPUT);
  Model1LowLevel::writeTEST(LOW);  // Deactivate TEST signal (release BUSREQ)
  delay(SETTLE_DELAY);

  // Verify TEST signal deactivation with multiple confirmations
  bool testDeactivated = false;
  uint16_t deactivationConfirms = 0;
  for (uint16_t i = 0; i < CONFIRM_LOOPS; i++) {
    delay(CONFIRM_DELAY);
    if (Model1LowLevel::readTEST() == HIGH) {
      deactivationConfirms++;
    }
  }

  if (deactivationConfirms >= (CONFIRM_LOOPS * 0.9)) {
    testDeactivated = true;
  }

  if (!testDeactivated) {
    M1Shield.setLEDColor(COLOR_RED);  // Critical failure
    setTextColor(0xF800, 0x0000);     // Red
    println(F(" FAILED"));
    println(F("ERROR: Cannot deactivate TEST"));
    println(F("signal! The TEST signal is"));
    println(F("stuck active (LOW). This"));
    println(F("prevents normal system"));
    println(F("operation and menu navigation."));
    println(F("The Z80 bus remains under"));
    println(F("external control."));
    println();
    setTextColor(0xFFFF, 0x0000);  // White
    println(F("Possible causes:"));
    println(F("- Short circuit on TEST line"));
    println(F("- GPIO pin stuck low"));
    println(F("- Z80 BUSACK not releasing"));
    println(F("- Hardware fault"));
    println();
    setTextColor(0xF800, 0x0000);  // Red
    println(F("*** SYSTEM HALTED ***"));
    println(F("Fix hardware before retrying"));
    println(F("Menu system will not work!"));
    println();
    setTextColor(0x07E0, 0x0000);  // Yellow
    println(F("Press RESET to restart"));

    // Block progression - endless loop with pulsing red LED
    // Menu system won't work with TEST stuck active
    while (true) {
      M1Shield.setLEDColor(COLOR_RED);
      delay(500);
      M1Shield.setLEDColor(COLOR_OFF);
      delay(500);
    }
  }
  println(F(" OK (inactive)"));

  setTextColor(0x07E0, 0x0000);  // Green
  println(F("TEST signal verification"));
  println(F("completed successfully."));
  
  setProgressValue(100);  // All tests complete

  // Final LED status indication
  bool overallPass = !dataResult.hasIssues && !addrResult.hasIssues && !crosstalkResult.hasIssues &&
                     !controlResult.hasIssues;

  // Show result
  if (overallPass) {
    M1Shield.setLEDColor(COLOR_GREEN);
  } else {
    M1Shield.setLEDColor(COLOR_YELLOW);
  }
}

Screen* DiagnosticConsole::actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) {
  if (action & BUTTON_ANY) {
    return new MainMenu();
  }

  return nullptr;
}
