#include "model1_th.h"

#include <Arduino.h>
#include <avr/pgmspace.h>

// Global memory buffer for read/write operations
uint8_t memoryBuffer[MEMORY_PAGE_SIZE];

// TRS-80 Model I VRAM
#define VRAM_BASE 0x3C00
#define VRAM_SIZE 0x0400

// Initialize all major subsystems for the test harness
void initHarness(uint8_t refresh_timer) {
  // Initialize Model1 and its subsystems, do not call any class methods that
  // access the Model1 hardware
  Model1.begin(refresh_timer);
  Model1.setLogger(logger);
  keyboard.setLogger(logger);
  rom.setLogger(logger);
  video.setLogger(logger);
  cassette.setLogger(logger);

  // Initialize the shield and set the global gfx pointer
  lcdInit();

  // turn on the TEST signal (active low), then do what you want
  Model1.activateTestSignal();

  // Clear the video display
  cassette.set64CharacterMode();
  video.cls();
  video.setAutoScroll(true);
  video.setLowerCaseMod(hasLowercaseInstalled());   // set lowercase mode if installed

  // TODO: update library to support all 3: TO_LCD, TO_LCD, TO_BOTH_DISPLAYS
  // print(TO_LCD, F("RetroStack Model 1 - Test Harness v"));
  // println(TO_LCD, VERSION);

  print(TO_LCD, F("RetroStack Model 1 - Test Harness v"));
  println(TO_LCD, VERSION);

  // video.print("RetroStack Model 1 - Test Harness v");
  // video.println(VERSION);

  // print out Model1 Arduino lib version
  println(TO_LCD, F("Model I Arduino Library v"), Model1.getVersion());
}

// Reset all major subsystems for the test harness
void resetHarness() {
  Model1.end();
  initHarness();

  // Force cache invalidation after reset
  invalidateStateCache();
}

// Toggle the TEST signal
void toggleTestSignal() {
  bool isHigh;

  if (1) {
    // Use real Model1.getState()
    isHigh = isSignalHigh("TEST", Model1.getState());
  } else {
    // Use mock Model1TH::getState()
    isHigh = isSignalHigh("TEST", Model1TH::getState());
  }

  // TEST signal is active-low:
  // - activateTestSignal() sets TEST to LOW (0) = active
  // - deactivateTestSignal() sets TEST to HIGH (1) = inactive
  if (isHigh)  // TEST is HIGH (1) = inactive
  {
    Model1.activateTestSignal();  // Make it LOW (0) = active
  } else                          // TEST is LOW (0) = active
  {
    Model1.deactivateTestSignal();  // Make it HIGH (1) = inactive
  }
}

// Function to force cache invalidation when state changes
void invalidateStateCache() {
  // No-op: caching has been removed, but keeping function for compatibility
}

void printTestSignalState() {
  print(TO_LCD, F("T) Toggle TEST* signal ["));

  // Switch: 0 = use Model1TH::getState() (mock), 1 = use Model1.getState() (real)
  const char *currentState;

  if (1) {
    // Use real Model1.getState()
    currentState = Model1.getState();
  } else {
    // Use mock Model1TH::getState()
    currentState = Model1TH::getState();
  }

  if (!currentState) {
    print(TO_LCD, F("ERROR: NULL state"));
    println(TO_LCD, F("]"));
    return;
  }

  // Debug: Check state length to catch issues early
  size_t stateLen = strlen(currentState);
  if (stateLen == 0) {
    print(TO_LCD, F("ERROR: Empty state"));
    println(TO_LCD, F("]"));
    return;
  }
  if (stateLen > 1000) {  // Sanity check for huge states
    print(TO_LCD, F("ERROR: State too long ("));
    print(stateLen);
    print(TO_LCD, F(" chars)"));
    println(TO_LCD, F("]"));
    return;
  }

  // Get the signal state and display result
  bool isHigh = isSignalHigh("TEST", currentState);

  if (isHigh) {
    print(TO_LCD, F("HIGH"));
  } else {
    print(TO_LCD, F("LOW"));
  }

  println(TO_LCD, F("]"));
}

void printModel1State(const char *state) {
  if (!state) {
    println(TO_LCD, F("<null>"));
    return;
  }
  const char *p = state;
  while (*p) {
    const char *next = strchr(p, ',');
    size_t len = next ? (size_t)(next - p) : strlen(p);
    // Copy and trim left
    static char token[128];  // Make static to avoid stack allocation
    size_t tlen = len < sizeof(token) - 1 ? len : sizeof(token) - 1;
    strncpy(token, p, tlen);
    token[tlen] = '\0';
    char *trimmed = token;
    while (*trimmed == ' ' || *trimmed == '\t' || *trimmed == '\r' || *trimmed == '\n')
      ++trimmed;
    println(TO_LCD, trimmed);
    if (!next)
      break;
    p = next + 1;
  }
}

// File-scope signal name constants for reuse
static const char RD[] = "RD";
static const char WR[] = "WR";
static const char IN[] = "IN";
static const char OUT[] = "OUT";
static const char RAS[] = "RAS";
static const char CAS[] = "CAS";
static const char MUX[] = "MUX";
static const char INT[] = "INT";
static const char TEST[] = "TEST";
static const char WAIT[] = "WAIT";

// Helper: expand signal name to human readable
const char *signalNameToHuman(const char *abbr) {
  if (strcmp(abbr, RD) == 0)
    return "Read";
  if (strcmp(abbr, WR) == 0)
    return "Write";
  if (strcmp(abbr, IN) == 0)
    return "Input";
  if (strcmp(abbr, OUT) == 0)
    return "Output";
  if (strcmp(abbr, RAS) == 0)
    return "Row Address Strobe";
  if (strcmp(abbr, CAS) == 0)
    return "Column Address Strobe";
  if (strcmp(abbr, MUX) == 0)
    return "Multiplexer";
  if (strcmp(abbr, INT) == 0)
    return "Interrupt";
  if (strcmp(abbr, TEST) == 0)
    return "Test Signal";
  if (strcmp(abbr, WAIT) == 0)
    return "Wait";
  return abbr;
}

// Parse state string to extract any signal state and return formatted result
// Returns: pointer to static buffer with formatted signal state, or NULL if not found
// Buffer format: "Signal Name: value" (max 25 chars)
const char *parseSignalState(const char *signal, const char *state) {
  static char resultBuffer[26];  // 25 chars + null terminator

  if (!signal || !state) {
    return NULL;
  }

  // Look for pattern: SIGNAL<...>(value)
  const char *p = state;
  size_t siglen = strlen(signal);

  while (*p) {
    // Find next comma or end
    const char *next = strchr(p, ',');
    size_t len = next ? (size_t)(next - p) : strlen(p);

    // Copy token and trim left
    static char token[128];
    size_t tlen = len < sizeof(token) - 1 ? len : sizeof(token) - 1;
    strncpy(token, p, tlen);
    token[tlen] = '\0';
    char *trimmed = token;
    while (*trimmed == ' ' || *trimmed == '\t' || *trimmed == '\r' || *trimmed == '\n')
      ++trimmed;

    // Look for pattern: SIGNAL<...>(value)
    const char *lt = strchr(trimmed, '<');
    const char *gt = lt ? strchr(lt + 1, '>') : nullptr;
    const char *lp = strchr(trimmed, '(');
    const char *rp = lp ? strchr(lp + 1, ')') : nullptr;

    if (lt && gt && lt > trimmed) {
      // abbr is [trimmed .. lt)
      size_t ablen = lt - trimmed;
      if (ablen == siglen && strncmp(trimmed, signal, siglen) == 0) {
        // Found the signal - get value if present
        const char *humanName = signalNameToHuman(signal);
        if (lp && rp && lp > gt && rp > lp) {
          // Extract value between parentheses
          size_t valueLen = rp - lp - 1;
          char value[16];
          size_t copyLen = valueLen < sizeof(value) - 1 ? valueLen : sizeof(value) - 1;
          strncpy(value, lp + 1, copyLen);
          value[copyLen] = '\0';

          snprintf(resultBuffer, sizeof(resultBuffer), "%s: %s", humanName, value);
        } else {
          snprintf(resultBuffer, sizeof(resultBuffer), "%s", humanName);
        }
        return resultBuffer;
      }
    }

    if (!next)
      break;
    p = next + 1;
  }

  // Signal not found
  return NULL;
}

// Helper function to check if a signal is HIGH (1) based on parsed result
// Returns: true if signal value is "1", false otherwise
bool isSignalHigh(const char *signal, const char *state) {
  const char *result = parseSignalState(signal, state);
  if (!result)
    return false;

  // Look for ": 1" pattern in the result
  const char *colon = strchr(result, ':');
  if (colon) {
    const char *value = colon + 1;
    // Skip whitespace
    while (*value == ' ')
      value++;
    return (*value == '1');
  }

  return false;
}

namespace Model1TH {

// ----------------------
// Mock state function for testing
// ----------------------

const char *getState() {
  return "Mut<T>,RfshEn<T>,RfshRow<  0>,RD<o>(1),WR<o>(1),IN<o>(1),OUT<o>(1),ADDR<o o-w>(00111100 "
         "00100111),DATA<i-r>(00111001),RAS<o>(1),CAS<o>(1),MUX<o>(0),SYS_RES<i>(1),INT_ACK<i>(1),"
         "INT<o>(1),TEST<o>(0),WAIT<o>(1)";
}

// ----------------------
// Internal helpers
// ----------------------

static inline const char *trim_left(const char *s) {
  while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n')
    ++s;
  return s;
}

static inline void rtrim(char *s) {
  size_t n = strlen(s);
  while (n && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n')) {
    s[--n] = '\0';
  }
}

/**
 * Safe copy (always null-terminates).
 */
static inline void clamp_copy(char *dst, size_t dst_sz, const char *src, size_t len) {
  if (dst_sz == 0)
    return;
  if (len >= dst_sz)
    len = dst_sz - 1;
  memcpy(dst, src, len);
  dst[len] = '\0';
}

/**
 * Map direction / attribute tokens to a human string.
 * Anything unknown falls back to the raw token printed without the angle brackets.
 */
static const char *dirToString(const char *dir) {
  // Normalize by stripping surrounding '<' '>' if present
  static char buf[24];
  size_t dlen = strlen(dir);
  const char *p = dir;
  if (dlen >= 2 && dir[0] == '<' && dir[dlen - 1] == '>') {
    ++p;
    dlen -= 2;
  }
  if (dlen >= sizeof(buf))
    dlen = sizeof(buf) - 1;
  memcpy(buf, p, dlen);
  buf[dlen] = '\0';

  // Now match normalized values
  if (strcmp(buf, "o") == 0)
    return "Output";
  if (strcmp(buf, "i") == 0)
    return "Input";
  if (strcmp(buf, "i-r") == 0)
    return "Input-Read";
  if (strcmp(buf, "i-w") == 0)
    return "Input-Write";
  if (strcmp(buf, "o-w") == 0)
    return "Output-Write";
  if (strcmp(buf, "o-r") == 0)
    return "Output-Read";
  if (strcmp(buf, "T") == 0)
    return "True";
  if (strcmp(buf, "F") == 0)
    return "False";
  if (strcmp(buf, "o o-w") == 0)
    return "Output-Write";  // as seen in ADDR<o o-w>
  // RfshRow<  0> (numeric) or any other oddballs -> return raw without < >
  return buf;
}

/**
 * Expand short signal names into the labels you showed in your sample output.
 * Unrecognized names are returned as-is.
 */
static const char *expandSignal(const char *abbr) {
  if (strcmp(abbr, RD) == 0)
    return "RD*";
  if (strcmp(abbr, WR) == 0)
    return "WR*";
  if (strcmp(abbr, IN) == 0)
    return "IN*";
  if (strcmp(abbr, OUT) == 0)
    return "OUT*";
  if (strcmp(abbr, RAS) == 0)
    return "RAS*";
  if (strcmp(abbr, CAS) == 0)
    return "CAS*";
  if (strcmp(abbr, MUX) == 0)
    return "MUX*";
  if (strcmp(abbr, INT) == 0)
    return "INT*";
  if (strcmp(abbr, TEST) == 0)
    return "TEST*";
  if (strcmp(abbr, WAIT) == 0)
    return "WAIT*";

  // Keep original for the rest
  return abbr;
}

/**
 * Try to parse:  <abbr><dir>(value)
 * Returns true if it matched that pattern.
 * If no "(value)" is present we still might want to just print the raw token.
 */
static bool parseTriplet(const char *token, char *abbr, size_t abbr_sz, char *dir, size_t dir_sz,
                         char *val, size_t val_sz) {
  abbr[0] = dir[0] = val[0] = '\0';

  const char *lt = strchr(token, '<');
  const char *gt = lt ? strchr(lt + 1, '>') : nullptr;
  const char *lp = strchr(token, '(');
  const char *rp = lp ? strchr(lp + 1, ')') : nullptr;

  if (!lt || !gt || lt <= token) {
    // no abbr<...>, so not the pattern we expect
    return false;
  }

  // abbr is [token .. lt)
  clamp_copy(abbr, abbr_sz, token, (size_t)(lt - token));
  rtrim(abbr);

  // dir is [lt .. gt] inclusive
  clamp_copy(dir, dir_sz, lt, (size_t)(gt - lt + 1));

  // If we have a (value), capture it.
  if (lp && rp && lp > gt && rp > lp) {
    clamp_copy(val, val_sz, lp + 1, (size_t)(rp - lp - 1));
    return true;
  }

  // Still a valid parse (abbr<dir>) but no (value)
  return true;
}

}  // namespace Model1TH

void printModel1StateNice(const char *state) {
  if (!state) {
    println(TO_LCD, F("Model State: <null>"));
    return;
  }

  println(TO_LCD, F("Model State:"));

  const char *p = state;
  uint8_t count = 0;

  while (*p) {
    const char *next = strchr(p, ',');
    size_t len = next ? (size_t)(next - p) : strlen(p);

    // Work on a local, mutable buffer
    static char token[128];  // Make static to avoid stack allocation
    Model1TH::clamp_copy(token, sizeof(token), p, len);

    const char *trimmed = Model1TH::trim_left(token);

    static char abbr[24], dir[24], val[64];  // Make static to avoid stack allocation
    bool ok =
        Model1TH::parseTriplet(trimmed, abbr, sizeof(abbr), dir, sizeof(dir), val, sizeof(val));

    if (ok && abbr[0] != '\0') {
      // If we have a recognized abbr<dir>, print pretty
      print(TO_LCD, F("  "));
      print(Model1TH::expandSignal(abbr));

      // If we had a (value), we want a " = value" line
      const char *dirPretty = Model1TH::dirToString(dir);
      if (val[0] != '\0') {
        print(TO_LCD, F(" ("));
        print(dirPretty);
        print(TO_LCD, F(") = "));
        println(val);
      } else {
        // No (value) -> print whatever we can in a raw-ish way
        print(TO_LCD, F(" <"));
        print(dirPretty);
        println(TO_LCD, F(">"));
      }
    } else {
      // Fall back to raw token
      print(TO_LCD, F("  "));
      println(trimmed);
    }

    ++count;

    if (!next)
      break;
    p = next + 1;
  }

  //   println(TO_LCD, F("--------"));
  //   print(TO_LCD, F("Parsed states: "));
  //   println(count);
}

// Show system configuration
void systemConfiguration() {
  bool hasLowercase = hasLowercaseInstalled();
  if (hasLowercase) {
    video.setLowerCaseMod(true);
  }
  video.cls();
  lcdClear(ST77XX_BLACK);

  printSeparator(TO_LCD, F("[SYS CONF] System configuration"), '.', 52, 0, 5);
  println(TO_LCD, F("ROM version: "), rom.identifyROM());
  println(TO_LCD, F("RAM size   : "), getMemorySize());
  println(TO_LCD, F("Lowercase  :"), hasLowercase ? F("Yes") : F("No"));
  println(TO_LCD, F("Video in 64 or 32 mode: "), cassette.is64CharacterMode() ? F("64") : F("32"));
  println(TO_LCD, F("Font : look at display"));
  printFont();
  printSeparator(TO_LCD, F("end"), '.', 52, 0);
  inputPrompt(TO_LCD, F("\nPress any key to continue..."));
  println(TO_LCD, F(""));
}

void printFont() {

  // video.print() doesn't seem to support graphics characters, so we use writeMemory
  // TODO: look into this
  for (uint8_t i = 0; i < 255; i++) {
    Model1.writeMemory(VRAM_BASE + (64 * 8) + i, i);  // go down 8 lines
  }
}

// optimize version of ORIG
uint16_t getMemorySize_orig_opt() {
  constexpr uint16_t PAGE = 0x0100;  // 256-byte page
  constexpr uint8_t MAX_BAD = 10;

  /* -- Testable RAM spans (high->low) ------------------------------
     Stored in PROGMEM so they never touch SRAM or the stack.     */
  static const uint16_t spanTop[] PROGMEM = {0xFFFF, 0xBFFF, 0x7FFF, 0x4FFF};
  static const uint16_t spanBottom[] PROGMEM = {0xC000, 0x8000, 0x5000, 0x4000};
  constexpr uint8_t SPAN_CNT = sizeof(spanTop) / sizeof(spanTop[0]);

  uint16_t highestGood = 0;
  uint8_t badCount = 0;
  uint16_t badPages[MAX_BAD];

  /* -- Probe every page ------------------------------------------ */
  for (uint8_t i = 0; i < SPAN_CNT; ++i) {
    uint16_t top = pgm_read_word(spanTop + i);
    uint16_t bottom = pgm_read_word(spanBottom + i);

    for (uint16_t addr = top;; addr -= PAGE) {
      uint8_t orig = Model1.readMemory(addr);
      uint8_t patt = orig ^ 0xFF;  // faster/smaller than '~'

      Model1.writeMemory(addr, patt);
      bool ok = (Model1.readMemory(addr) == patt);
      Model1.writeMemory(addr, orig);  // restore

      if (!ok) {  // first failure => stop
        if (badCount < MAX_BAD)
          badPages[badCount++] = addr;
        goto finished;
      }
      highestGood = addr;
      if (addr == bottom)
        break;
    }
  }

finished:
  uint16_t bytes = highestGood ? uint16_t((highestGood & 0xFF00U) + PAGE) : 0;

  /* -- Report ---------------------------------------------------- */
  if (badCount) {
    println(TO_LCD, F("Bad page(s): "), badCount);
    for (uint8_t i = 0; i < badCount; ++i)
      println(TO_LCD, F("  0x"), badPages[i], Hex);
  } else {
    println(TO_LCD, F("No bad pages detected"));
  }
  println(TO_LCD, F("RAM size   : "), bytes);  // 0, 4 K, 16 K, 32 K, 48 K
  return bytes;
}

// AI generated function to get memory size - ORIG
uint16_t getMemorySize() {
  constexpr uint16_t PAGE = 0x0100;  // probe one 256-byte page
  constexpr uint8_t MAX_BAD = 10;    // how many pages to list

  // Testable RAM ranges, high->low order
  struct Span {
    uint16_t top;
    uint16_t bottom;
  };
  const Span spans[] = {
      {0xFFFF, 0xC000},  // 48 K bank (16 K)
      {0xBFFF, 0x8000},  // 32 K bank (16 K)
      {0x7FFF, 0x5000},  // 16 K bank (12 K)
      {0x4FFF, 0x4000}   // 4 K bank  ( 4 K)
  };

  uint16_t highestGood = 0;
  uint8_t badCount = 0;
  uint16_t badPages[MAX_BAD];

  for (const Span &s : spans) {
    for (uint16_t addr = s.top; addr >= s.bottom; addr -= PAGE) {
      uint8_t orig = Model1.readMemory(addr);
      uint8_t test = ~orig;  // simple invert pattern

      Model1.writeMemory(addr, test);
      bool ok = (Model1.readMemory(addr) == test);
      Model1.writeMemory(addr, orig);  // restore byte

      if (!ok) {
        if (badCount < MAX_BAD)
          badPages[badCount++] = addr;
        // first failure within this span -> RAM ends just above it
        goto finished;
      }

      highestGood = addr;
      if (addr == s.bottom)
        break;  // reached end of span
    }
  }
finished:

  uint32_t bytes = (highestGood ? (highestGood & 0xFF00) + PAGE : 0);

  // --- Reporting -------------------------------------------------
  if (badCount) {
    println(TO_LCD, F("Bad page(s): "), badCount);
    for (uint8_t i = 0; i < badCount; ++i) {
      println(TO_LCD, F("  0x"), badPages[i], Hex);
    }
  } else {
    println(TO_LCD, F("No bad pages detected"));
  }

  // println(TO_LCD, F("RAM size   : "), bytes);
  // not rounding to familiar 4K, 16K, 32K, 48K (TODO)
  return (uint16_t)bytes;  // 0, 4096, 16384, 32768 or 49152 (4K, 16K, 32K or 48K)
}

// ROM algo for memory size detection
uint16_t getMemorySize_rom() {
#define MAX_BAD 10       // Maximum number of bad addresses to report
  uint16_t memSize = 0;  // retval of 0 should be considered an error
  uint8_t a = 0;         // accumulator for read/write operations
  uint8_t badCount = 0;  // Count of bad addresses

  for (uint16_t hl = 0xFFFF; hl >= 0x0100; hl -= 0x0100) {
    a = Model1.readMemory(hl);  // Read memory at HL

    a = ~a;  // CPL
    Model1.writeMemory(hl, a);

    if ((a ^ Model1.readMemory(hl)) != 0) {  // XOR (HL)
      badCount++;
      // TODO: update utils_th.h to handle unsigned long integers
      // println(TO_LCD, F("[FAIL] Bad address: 0x"), HL, Hex, F(" (expected: 0x"), A, F(")"), Hex);
      print(TO_LCD, F("[FAIL] Bad address: 0x"));
      print(hl, HEX);
      print(TO_LCD, F(" (expected: 0x"));
      print(a, HEX);
      println(TO_LCD, F(")"));
      memSize = hl;  // Set memSize to the last bad address
    } else {
      // Memory is good, set highest working address
      print(TO_LCD, F("[OK] Good address: 0x"));
      println(hl, HEX);
      // TODO: program is supposed to end here
    }
  }

  println(TO_LCD, F("Bad page addresses: "), badCount);
  print(TO_LCD, F("Memory size: "));
  print(memSize, HEX);
  return memSize;
}

bool hasLowercaseInstalled() {
  const uint16_t addrs[5] = {VRAM_BASE + 0x000, VRAM_BASE + 0x055, VRAM_BASE + 0x0AA,
                             VRAM_BASE + 0x100, VRAM_BASE + 0x155};
  const uint8_t BIT6 = 0x40;

  uint8_t matches = 0;

  for (uint8_t i = 0; i < 5; i++) {
    uint16_t a = addrs[i] % (VRAM_BASE + VRAM_SIZE);  // safety if you tweak list
    uint8_t orig = Model1.readMemory(a);
    uint8_t test = orig | BIT6;

    Model1.writeMemory(a, test);
    uint8_t rb = Model1.readMemory(a);

    if ((rb & BIT6) == BIT6)
      matches++;

    Model1.writeMemory(a, orig);  // restore
  }

  return matches >= 3;
}

// Function to write data to memory and verify
void writeMemoryData(const uint8_t *data, size_t dataSize, uint16_t startAddress, bool verify) {
  println(TO_LCD, F("=== Memory Write/Read Test ==="));
  print(TO_LCD, F("Data size: "));
  print(dataSize);
  print(TO_LCD, F(" bytes, Starting address: 0x"));
  println(startAddress, HEX);
  print(TO_LCD, F("Verify mode: "));
  println(verify ? F("ON") : F("OFF"));
  println(TO_LCD, F("Writing to memory..."));

  // Write each byte to memory
  uint16_t address = startAddress;
  for (size_t i = 0; i < dataSize; i++) {
    uint8_t dataByte = data[i];
    Model1.writeMemory(address, dataByte);
    print(TO_LCD, F("Wrote 0x"));
    print(dataByte, HEX);
    if (dataByte >= 32 && dataByte <= 126) {  // Printable ASCII
      print(TO_LCD, F(" ('"));
      print((char)dataByte);
      print(TO_LCD, F("')"));
    }
    print(TO_LCD, F(" to address 0x"));
    println(address, HEX);
    address++;
  }

  println(TO_LCD, F("Reading back from memory..."));

  // Read back and print
  address = startAddress;
  print(TO_LCD, F("Read back data: "));
  for (size_t i = 0; i < dataSize; i++) {
    uint8_t readByte = Model1.readMemory(address);
    if (readByte >= 32 && readByte <= 126) {  // Printable ASCII
      print((char)readByte);
    } else {
      print(TO_LCD, F("[0x"));
      print(readByte, HEX);
      print(TO_LCD, F("]"));
    }
    address++;
  }
  println();

  // Detailed byte-by-byte comparison
  println(TO_LCD, F("Byte-by-byte comparison:"));
  address = startAddress;
  for (size_t i = 0; i < dataSize; i++) {
    uint8_t originalByte = data[i];
    uint8_t readByte = Model1.readMemory(address);
    print(TO_LCD, F("Address 0x"));
    print(address, HEX);
    print(TO_LCD, F(": Original=0x"));
    print(originalByte, HEX);
    print(TO_LCD, F(" Read=0x"));
    print(readByte, HEX);
    print(TO_LCD, F(" "));
    println(originalByte == readByte ? F("MATCH") : F("MISMATCH"));
    address++;
  }

  // Additional verification using readMemoryData if verify flag is set
  if (verify) {
    println(TO_LCD, F("\n=== Enhanced Verification ==="));
    println(TO_LCD, F("Calling readMemoryData() for full page context..."));
    uint8_t *pageData = readMemoryData(startAddress, MEMORY_PAGE_SIZE);
    printMemoryData(pageData, startAddress, MEMORY_PAGE_SIZE);

    // Overall verification status
    println(TO_LCD, F("--- Final Verification Result ---"));
    address = startAddress;
    bool allMatch = true;
    for (size_t i = 0; i < dataSize; i++) {
      uint8_t originalByte = data[i];
      uint8_t readByte = Model1.readMemory(address);
      if (originalByte != readByte) {
        allMatch = false;
        break;
      }
      address++;
    }

    if (allMatch) {
      println(TO_LCD, F("SUCCESS: All data verified correctly!"));
    } else {
      println(TO_LCD, F("FAILURE: Data verification failed!"));
    }
  }
}

// Function to read memory data and return pointer to buffer
uint8_t *readMemoryData(uint16_t address, uint16_t size) {
  // Limit size to maximum buffer capacity
  if (size > MEMORY_PAGE_SIZE) {
    size = MEMORY_PAGE_SIZE;
    print(TO_LCD, F("Warning: Size limited to "));
    print(MEMORY_PAGE_SIZE);
    println(TO_LCD, F(" bytes"));
  }

  // Read the specified number of bytes into our global buffer
  for (uint16_t i = 0; i < size; i++) {
    memoryBuffer[i] = Model1.readMemory(address + i);
  }

  // Clear unused portion of buffer (optional, for debugging)
  for (uint16_t i = size; i < MEMORY_PAGE_SIZE; i++) {
    memoryBuffer[i] = 0;
  }

  return memoryBuffer;
}

// Function to get direct access to the global memory buffer
uint8_t *getMemoryBuffer() {
  return memoryBuffer;
}

// Function to print memory data from buffer (helper for readMemoryData)
void printMemoryData(const uint8_t *buffer, uint16_t address, uint16_t size) {
  println(TO_LCD, F("=== Memory Data Display ==="));
  print(TO_LCD, F("Reading "));
  print(size);
  print(TO_LCD, F(" bytes starting from address: 0x"));
  println(address, HEX);

  // Calculate how many rows we need (16 bytes per row)
  uint16_t rows = (size + 15) / 16;  // Round up division

  println(TO_LCD, F("Memory contents:"));
  println(TO_LCD, F("Addr  +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F  ASCII"));
  println(TO_LCD, F("----  -----------------------------------------------  ----------------"));

  // Display in hex dump format (16 bytes per line)
  for (uint16_t row = 0; row < rows; row++) {
    uint16_t rowAddress = address + (row * 16);
    uint16_t bytesInRow = (row == rows - 1) ? (size - row * 16) : 16;

    // Ensure we don't go beyond the requested size
    if (bytesInRow > 16)
      bytesInRow = 16;
    if (row * 16 >= size)
      break;

    // Print address
    print(TO_LCD, F(""));
    if (rowAddress < 0x1000)
      print(TO_LCD, F("0"));
    if (rowAddress < 0x100)
      print(TO_LCD, F("0"));
    if (rowAddress < 0x10)
      print(TO_LCD, F("0"));
    print(rowAddress, HEX);
    print(TO_LCD, F("  "));

    // Print hex values
    for (uint8_t col = 0; col < 16; col++) {
      if (col < bytesInRow) {
        uint8_t byte = buffer[row * 16 + col];
        if (byte < 0x10)
          print(TO_LCD, F("0"));
        print(byte, HEX);
      } else {
        print(TO_LCD, F("  "));  // Empty space for bytes beyond size
      }
      print(TO_LCD, F(" "));
    }

    print(TO_LCD, F(" "));

    // Print ASCII representation
    for (uint8_t col = 0; col < 16; col++) {
      if (col < bytesInRow) {
        uint8_t byte = buffer[row * 16 + col];
        if (byte >= 32 && byte <= 126) {
          print((char)byte);
        } else {
          print(TO_LCD, F("."));
        }
      } else {
        print(TO_LCD, F(" "));  // Empty space for bytes beyond size
      }
    }

    println();
  }

  println();
  print(TO_LCD, F("Buffer stored at: 0x"));
  println((uintptr_t)buffer, HEX);
  print(TO_LCD, F("Requested size: "));
  print(size);
  println(TO_LCD, F(" bytes"));
}

// Function to copy PROGMEM pattern to RAM buffer (helper for PROGMEM patterns)
void copyProgmemPattern(const uint8_t *progmemPattern, uint8_t *ramBuffer, uint16_t size) {
  if (size > MEMORY_PAGE_SIZE) {
    size = MEMORY_PAGE_SIZE;
    print(TO_LCD, F("Warning: Size limited to "));
    print(MEMORY_PAGE_SIZE);
    println(TO_LCD, F(" bytes"));
  }

  for (uint16_t i = 0; i < size; i++) {
    ramBuffer[i] = pgm_read_byte(&progmemPattern[i]);
  }

  // Clear unused portion if size < MEMORY_PAGE_SIZE
  for (uint16_t i = size; i < MEMORY_PAGE_SIZE; i++) {
    ramBuffer[i] = 0;
  }
}

// Function to write PROGMEM pattern directly to memory
void writeProgmemPatternToMemory(const uint8_t *progmemPattern, uint16_t address, uint16_t size,
                                 bool verify) {
  println(TO_LCD, F("=== Writing PROGMEM Pattern to Memory ==="));
  print(TO_LCD, F("Pattern size: "));
  print(size);
  print(TO_LCD, F(" bytes, Starting address: 0x"));
  println(address, HEX);
  print(TO_LCD, F("Verify mode: "));
  println(verify ? F("ON") : F("OFF"));
  println(TO_LCD, F("Writing PROGMEM pattern to memory..."));

  if (size > MEMORY_PAGE_SIZE) {
    size = MEMORY_PAGE_SIZE;
    print(TO_LCD, F("Warning: Size limited to "));
    print(MEMORY_PAGE_SIZE);
    println(TO_LCD, F(" bytes"));
  }

  // Write each byte from PROGMEM to memory
  for (uint16_t i = 0; i < size; i++) {
    uint8_t dataByte = pgm_read_byte(&progmemPattern[i]);
    Model1.writeMemory(address + i, dataByte);

    if (i < 16 || (i % 32) == 0) {  // Show first 16 bytes and every 32nd byte
      print(TO_LCD, F("Wrote 0x"));
      print(dataByte, HEX);
      print(TO_LCD, F(" to address 0x"));
      println(address + i, HEX);
    }
  }

  if (size > 16) {
    print(TO_LCD, F("... ("));
    print(size - 16);
    println(TO_LCD, F(" more bytes written)"));
  }

  // Additional verification using readMemoryData if verify flag is set
  if (verify) {
    println(TO_LCD, F("\n=== PROGMEM Pattern Verification ==="));
    println(TO_LCD, F("Reading back data for verification..."));
    uint8_t *readBack = readMemoryData(address, size);

    bool allMatch = true;
    uint16_t mismatches = 0;

    for (uint16_t i = 0; i < size; i++) {
      uint8_t originalByte = pgm_read_byte(&progmemPattern[i]);
      uint8_t readByte = readBack[i];

      if (originalByte != readByte) {
        allMatch = false;
        mismatches++;

        // Show first few mismatches
        if (mismatches <= 5) {
          print(TO_LCD, F("MISMATCH at 0x"));
          print(address + i, HEX);
          print(TO_LCD, F(": Expected 0x"));
          print(originalByte, HEX);
          print(TO_LCD, F(", Got 0x"));
          println(readByte, HEX);
        }
      }
    }

    if (mismatches > 5) {
      print(TO_LCD, F("... and "));
      print(mismatches - 5);
      println(TO_LCD, F(" more mismatches"));
    }

    println(TO_LCD, F("--- Final Verification Result ---"));
    if (allMatch) {
      println(TO_LCD, F("SUCCESS: PROGMEM pattern verified correctly!"));
    } else {
      print(TO_LCD, F("FAILURE: "));
      print(mismatches);
      print(TO_LCD, F(" out of "));
      print(size);
      println(TO_LCD, F(" bytes mismatched!"));
    }

    // Display formatted output
    println(TO_LCD, F("\n=== Memory Contents After PROGMEM Write ==="));
    printMemoryData(readBack, address, size);
  }
}

void triggerInterrupt() {
  println(TO_LCD, F("=== Triggering Interrupt ==="));
  Model1.activateInterruptRequestSignal();
  println(TO_LCD, F("IRQ generated. Waiting for INTAK*"));

  // TODO: may have to avoid the library
  const unsigned long start = millis();  // mark start time
  const unsigned long timeoutMs = 1000;  // 1 000 ms timeout

  while (!Model1.readInterruptAcknowledgeSignal()) {
    if (millis() - start >= timeoutMs) {
      println(TO_LCD, F("*ERR* Timeout waiting for INTAK*"));
      return;  // give up after 1 s
    }
    delay(10);  // small sleep to avoid busy-wait
  }

  println(TO_LCD, F("Interrupt triggered!"));
}

void toggleWaitSignal() {
  println(TO_LCD, F("=== Toggling Wait Signal ==="));
  Model1.activateWaitSignal();
  println(TO_LCD, F("Wait signal toggled!"));
}

void waitSignal() {
  toggleWaitSignal();
  print(TO_LCD, F("WAIT* signal toggled ["));
  // Switch: 0 = use Model1TH::getState() (mock), 1 = use Model1.getState() (real)
  bool isHigh;

  if (0) {
    // Use real Model1.getState()
    const char *currentState = Model1.getState();

    // Debug: Show the raw state string
    print(TO_LCD, F("Raw state: "));
    println(currentState);

    // Debug: Show what parseSignalState returns
    const char *parsed = parseSignalState("WAIT", currentState);
    if (parsed) {
      print(TO_LCD, F("Parsed WAIT: "));
      println(parsed);
    } else {
      println(TO_LCD, F("Parse failed - WAIT not found"));
    }

    isHigh = isSignalHigh("WAIT", currentState);
  } else {
    // Use mock Model1TH::getState()
    isHigh = isSignalHigh("WAIT", Model1TH::getState());
  }

  if (isHigh) {
    print(TO_LCD, F("HIGH"));
  } else {
    print(TO_LCD, F("LOW"));
  }

  println(TO_LCD, F("]"));
}
