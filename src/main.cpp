
#include <Arduino.h>

#include "globals_th.h"
#include "menu_th.h"

const char VERSION[] = "1.0.10";  // Define the version variable

void setup0();
void setup1();
void setup2();
void setup3();
void setup4();
void setup5();

void setup5() {
  Serial.begin(500000);  // Initialize serial communication
  initHarness(2);

  // video print
  video.cls();
  for (uint8_t i = 0; i <= 250; ++i) {
    // send to serial
    // Serial.print(i);
    // write to VRAM
    // Model1.writeMemory(VRAM_BASE + i, i);
    video.print(video.convertLocalCharacterToModel1(i));
  }

  while (1) {
    // Do nothing, just loop forever
  }
}
void setup4() {
  Serial.begin(500000);  // Initialize serial communication
  initHarness(2);
  Model1.begin();

  // Memory size detection algorithm
  Serial.println(F("\n=== Memory Size Detection ==="));
  Serial.println(F("Testing memory from 0xFFFF down to 0x0000..."));

  uint16_t address = 0xFFFF;
  uint16_t lastGoodAddress = 0x0000;

  while (address >= 0x0100) {  // Stop before we reach 0x0000 to avoid underflow
    // Read value
    uint8_t a = Model1.readMemory(address);

    // Take complement
    a = ~a;

    // Write complement to memory
    Model1.writeMemory(address, a);

    // XOR the complement with what we read back
    uint8_t xorResult = a ^ Model1.readMemory(address);

    if (xorResult == 0) {
      // Memory is good at this address
      Serial.print(F("Good memory at address: 0x"));
      Serial.println(address, HEX);
      lastGoodAddress = address;
      // break; // Stop at the first good address
    } else {
      // Memory failed at this address
      Serial.print(F("Memory failed at address: 0x"));
      Serial.print(address, HEX);
      Serial.print(F(" (XOR result: 0x"));
      Serial.print(xorResult, HEX);
      Serial.println(F(")"));
    }

    // Move to next page boundary (subtract 256 bytes)
    if (address < 0x0100)
      break;  // Prevent underflow
    address -= 0x0100;
  }

  // Calculate and report memory size
  uint32_t memorySize = lastGoodAddress;
  Serial.println(F("\n=== Memory Detection Results ==="));
  Serial.print(F("Highest good address: 0x"));
  Serial.println(lastGoodAddress, HEX);
  Serial.print(F("Detected memory size: "));
  Serial.print(memorySize);
  Serial.println(F(" bytes"));
  Serial.print(F("Memory size in KB: "));
  Serial.print(memorySize / 1024);
  Serial.println(F(" KB"));

  while (1) {}
}

void setup3() {
  Serial.begin(500000);
  lcdInit();
  // gfx->println(F("RetroStack Model 1 - Test Harness v"));
  print(TO_LCD, "Hello");
  println(TO_LCD, "world! This is a test of the LCD display.");
  print(TO_LCD, "Bye!");

  uint8_t a = 0;
  uint16_t hl = 0xFFFF;  // Start from the highest address

  Model1.begin(-1);

  a = Model1.readMemory(hl);  // Read memory at HL
  println(F("Initial memory read at 0x"), hl, Hex, F(": 0x"), a, Hex);
  a = ~a;  // CPL
  println(F("CPL result at 0x"), hl, Hex, F(": 0x"), a, Hex);
  Model1.writeMemory(hl, a);
  uint8_t r = Model1.readMemory(hl) ^ a;

  println(F("XOR result at 0x"), hl, Hex, F(": 0x"), r, Hex);

  while (1) {}
}

void setup2() {
  Serial.begin(500000);  // Initialize serial communication
  initHarness(2);

  // Test string to write to memory
  const char* testString = "Hello World!";
  uint16_t startAddress = 0xFF00;  // Starting memory address

  // Convert string to uint8_t array
  size_t stringLength = strlen(testString);
  uint8_t* dataArray = (uint8_t*)testString;  // Cast to uint8_t*

  Serial.print(F("Original string: "));
  Serial.println(testString);
  Serial.print(F("String length (without null): "));
  Serial.println(stringLength);

  writeMemoryData(dataArray, stringLength, startAddress, true);

  Serial.println(F("\n--- Additional test without verify ---"));
  writeMemoryData(dataArray, stringLength, startAddress + 32);

  Serial.println(F("\n--- Reading memory page for verification ---"));
  uint8_t* memoryData = readMemoryData(startAddress + 32, 16);  // Read 16 bytes (page size)
  printMemoryData(memoryData, startAddress + 32, 16);

  // Now you can use the memoryData for other purposes
  Serial.println(F("\n--- Using returned data for analysis ---"));
  Serial.print(F("First byte: 0x"));
  Serial.println(memoryData[0], HEX);
  Serial.print(F("String reconstruction (exact length): "));

  // Use exact string length instead of searching for null terminator
  for (size_t i = 0; i < stringLength && i < 16; i++) {
    if (memoryData[i] >= 32 && memoryData[i] <= 126) {
      Serial.print((char)memoryData[i]);
    }
  }
  Serial.println();

  // Test PROGMEM patterns
  Serial.println(F("\n=== PROGMEM Pattern Testing ==="));

  // Test Pattern 1 (ascending 0x00-0xFF)
  Serial.println(F("\n--- Testing PROGMEM Pattern 1 (Ascending) ---"));
  writeProgmemPatternToMemory(testPattern1, 0xFE00, 16, true);

  // Test Pattern 2 (descending 0xFF-0x00)
  Serial.println(F("\n--- Testing PROGMEM Pattern 2 (Descending) ---"));
  writeProgmemPatternToMemory(testPattern2, 0xFD00, 16, true);

  // Copy PROGMEM pattern to our global buffer for manipulation
  Serial.println(F("\n--- Copying PROGMEM Pattern to Global Buffer ---"));
  copyProgmemPattern(testPattern1, getMemoryBuffer(), 16);
  Serial.println(F("Pattern 1 copied to global buffer"));
  Serial.print(F("Buffer contents (all 16 bytes): "));
  uint8_t* buffer = getMemoryBuffer();
  for (int i = 0; i < 16; i++) {
    Serial.print(F("0x"));
    if (buffer[i] < 0x10)
      Serial.print(F("0"));
    Serial.print(buffer[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();

  // Modify the buffer and write it to memory
  Serial.println(F("\n--- Modifying Buffer and Writing to Memory ---"));
  buffer[0] = 0xAA;   // Modify first byte
  buffer[1] = 0xBB;   // Modify second byte
  buffer[15] = 0xCC;  // Modify last byte (15 instead of 255)

  writeMemoryData(buffer, 16, 0xFC00, true);

  // Test with different sizes
  Serial.println(F("\n--- Testing different read sizes ---"));
  uint8_t* smallRead = readMemoryData(startAddress, 16);  // Read only 16 bytes
  printMemoryData(smallRead, startAddress, 16);

  // Demonstrate direct buffer access
  Serial.println(F("\n--- Direct global buffer access ---"));
  uint8_t* globalBuffer = getMemoryBuffer();
  Serial.print(F("Global buffer address: 0x"));
  Serial.println((uintptr_t)globalBuffer, HEX);
  Serial.print(F("Buffer contents (all 16 bytes): "));
  for (int i = 0; i < MEMORY_PAGE_SIZE; i++) {
    Serial.print(F("0x"));
    if (globalBuffer[i] < 0x10)
      Serial.print(F("0"));
    Serial.print(globalBuffer[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();

  while (1) {}
}

void setup1() {
  Serial.begin(500000);  // baud rate
  println(F("RetroStack Model 1 - Test Harness v"), VERSION);

  Model1.begin();
  Model1.setLogger(logger);

  // Switch: 0 = use Model1TH::getState() (mock), 1 = use Model1.getState() (real)
  if (1) {
    // Use real Model1.getState()
    println(Model1.getState(), "-----");
    printModel1State(Model1.getState());
    println("-----");
    printModel1StateNice(Model1.getState());
    println("-----");

    // Use memory-safe version
    bool isHigh = isSignalHigh("TEST", Model1.getState());
    println(F("TEST signal is: "), isHigh ? F("HIGH") : F("LOW"));

    // Demonstrate parsing other signals
    const char* rdState = parseSignalState("RD", Model1.getState());
    if (rdState) {
      println(F("Parsed RD signal: "), rdState);
    }
    const char* wrState = parseSignalState("WR", Model1.getState());
    if (wrState) {
      println(F("Parsed WR signal: "), wrState);
    }
  } else {
    // Use mock Model1TH::getState()
    println(Model1TH::getState(), "-----");
    printModel1State(Model1TH::getState());
    println("-----");
    printModel1StateNice(Model1TH::getState());
    println("-----");

    // Use memory-safe version
    bool isHigh = isSignalHigh("TEST", Model1TH::getState());
    println(F("TEST signal is: "), isHigh ? F("HIGH") : F("LOW"));

    // Demonstrate parsing other signals
    const char* rdState = parseSignalState("RD", Model1TH::getState());
    if (rdState) {
      println(F("Parsed RD signal: "), rdState);
    }
    const char* wrState = parseSignalState("WR", Model1TH::getState());
    if (wrState) {
      println(F("Parsed WR signal: "), wrState);
    }
  }
}

// Prodution setup function
void setup0() {
  println(F("Loading..."));
  initHarness();  // Initialize the test harness
  Menu::init();
  Menu::showCurrent();
}

void setup() {
  Serial.begin(500000);  // Initialize serial communication

  int setup = 0;
  switch (setup) {
    case 0:
      Serial.println(F("Test Harness Setup 0"));
      setup0();
      break;
    case 1:
      Serial.println(F("Test Harness Setup 1"));
      setup1();
      break;
    case 2:
      Serial.println(F("Test Harness Setup 2"));
      setup2();
      break;
    case 3:
      Serial.println(F("Test Harness Setup 3"));
      setup3();
      break;
    case 4:
      Serial.println(F("Test Harness Setup 4"));
      setup4();
      break;
    case 5:
      Serial.println(F("Test Harness Setup 5"));
      setup5();
      break;
    default:
      Serial.println(F("YO! DUMMY - YOU MISSED CONIGURATION!"));
      break;
  }
}

void loop_() {}

void loop() {
  Menu::pollSerial();
}

// Define a callback for Timer 2
ISR(TIMER2_COMPA_vect) {
  // Trigger a refresh to happen
  Model1.nextUpdate();
}
