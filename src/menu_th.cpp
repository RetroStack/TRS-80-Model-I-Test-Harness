#include "menu_th.h"

// Include all module headers that menu needs to dispatch to
#include "cassette_th.h"
#include "keyboard_th.h"
#include "ports_th.h"
#include "ram_th.h"
#include "rom_th.h"
#include "sys_config_th.h"
#include "video_th.h"

// Helper function to get free memory on Arduino (outside namespace to avoid linker issues)
int getFreeMemory() {
#ifdef __AVR__
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
#else
  return -1;  // Not available on non-AVR platforms
#endif
}

namespace Menu {
NodeID currentMenu;

NodeID menuStack[MAX_MENU_DEPTH];
int menuDepth = 0;

void push(NodeID node) {
  if (menuDepth < MAX_MENU_DEPTH) {
    menuStack[++menuDepth] = node;
    // println(F("[DEBUG] Pushed NodeID:"));
    // println(static_cast<int>(node));
  }
}

void pop() {
  if (menuDepth > 0) {
    // println(F("[DEBUG] Popped NodeID:"));
    // println(static_cast<int>(menuStack[menuDepth]));
    --menuDepth;
  }
}

NodeID current() {
  return menuStack[menuDepth];
}

// Main menu display table stored in PROGMEM
struct MainMenuDisplay {
  char key;
  const char *description;
};

const char desc_video[] PROGMEM = "Video";
const char desc_memory[] PROGMEM = "Memory (RAM)";
const char desc_rom[] PROGMEM = "ROM";
const char desc_keyboard[] PROGMEM = "Keyboard";
const char desc_cassette[] PROGMEM = "Cassette";
const char desc_sysconfig[] PROGMEM = "System configuration";
const char desc_portutils[] PROGMEM = "I/O Port utilities";
const char desc_exit[] PROGMEM = "Exit";

const MainMenuDisplay mainMenuDisplay[] PROGMEM = {
    {'c', desc_cassette}, {'x', desc_exit}, {'p', desc_portutils}, {'k', desc_keyboard},
    {'m', desc_memory},   {'r', desc_rom},  {'s', desc_sysconfig}, {'v', desc_video}};

const uint8_t mainMenuDisplaySize = sizeof(mainMenuDisplay) / sizeof(MainMenuDisplay);

void showMain() {
  // Debug: Count how many times menu is shown
  static uint16_t menuCount = 0;  // Changed to 16-bit to avoid overflow
  menuCount++;
  print(F("[Menu #"));
  print(menuCount);
  print(F("] "));

  // Debug: Print free memory
  int freeMemory = getFreeMemory();
  if (freeMemory >= 0) {
    print(F("[Free Memory: "));
    print(freeMemory);
    println(F(" bytes]"));
  }

  // Display menu items from table
  for (uint8_t i = 0; i < mainMenuDisplaySize; i++) {
    MainMenuDisplay item;
    memcpy_P(&item, &mainMenuDisplay[i], sizeof(MainMenuDisplay));

    print(TO_LCD, item.key);
    print(TO_LCD, F(") "));
    println(TO_LCD, (__FlashStringHelper *)item.description);
  }

  // Display dynamic/special items
  printTestSignalState();
  println(TO_LCD, ("C) Clear screen"));
  println(TO_LCD, ("B) Bus pin statuses"));
  println(TO_LCD, ("b) Flag BUSAK* connected to Test Harness shield"));
  println(TO_LCD, ("I) Trigger INT (TODO)"));
  println(TO_LCD, ("W) Wait signal (TODO)"));
  println(TO_LCD, ("X) Reset harness application"));
  print(F("> "));
}

bool handleGlobalCommandBool(char c) {
  switch (c) {
    // Test signal toggle
    case 'T': {
      testSignal();
      return true;
    }

    // Bus pin statuses
    case 'B':
      clearScreen();  // TODO: need to figure out display scrolling
      // Switch: 0 = use Model1TH::getState() (mock), 1 = use Model1.getState() (real)
      // TODO: memory leak in Model1.getState()
      if (1) {
        // Use real Model1.getState()
        printModel1State(Model1.getState());
      } else {
        // Use mock Model1TH::getState()
        printModel1State(Model1TH::getState());
      }
      inputPrompt("Press ? for menu.");
      return true;
    case 'C':
      clearScreen();
      return true;
    case 'I':
      triggerInterrupt();
    case 'W':
      toggleWaitSignal();
      return true;
    case 'X':
      resetHarness();
      return true;
    default:
      // println(F("Invalid global command"));
      return false;
  }
}

/*  menu.cpp ──────────────────────────────────────────────────────
    Compact breadcrumb generator for AVR boards (e.g. Mega2560).
    All labels live in flash; only ~96 B stack used at runtime.
*/

/* ───────── Flash‑resident label strings ───────── */
const char lbl_MAIN[] PROGMEM = "MAIN";
const char lbl_VIDEO[] PROGMEM = "VIDEO";
const char lbl_VIDEO_TESTS[] PROGMEM = "TESTS";
const char lbl_RAM[] PROGMEM = "RAM";
const char lbl_RAM_TESTS[] PROGMEM = "RAM_TESTS";
const char lbl_ROM[] PROGMEM = "ROM";
const char lbl_KB[] PROGMEM = "KB";
const char lbl_CAS[] PROGMEM = "CAS";
const char lbl_CAS_SONGS[] PROGMEM = "SONGS";
const char lbl_SYS_CFG[] PROGMEM = "SYS_CONFIG";
const char lbl_PORT_UTILS[] PROGMEM = "PORTS";

/* Table of pointers to the above labels (also in flash) */
const char *const nodeLabelTable[] PROGMEM = {lbl_MAIN,      lbl_VIDEO,   lbl_VIDEO_TESTS, lbl_RAM,
                                              lbl_RAM_TESTS, lbl_ROM,     lbl_KB,          lbl_CAS,
                                              lbl_CAS_SONGS, lbl_SYS_CFG, lbl_PORT_UTILS};

/* Helper: fetch label address from PROGMEM */
static inline const char *getLabel(NodeID id) {
  return (const char *)pgm_read_word(&nodeLabelTable[(uint8_t)id]);
}

/* ───────── Public API ───────── */
void showBreadcrumb() {
  static char bc[96];  // Make static to avoid stack allocation
  uint8_t pos = 0;
  bc[pos++] = '[';

  for (uint8_t i = 0; i <= menuDepth && pos < sizeof(bc) - 1; ++i) {
    pos += snprintf_P(bc + pos, sizeof(bc) - pos, PSTR("%S"), getLabel(menuStack[i]));
    if (i < menuDepth) {
      pos += snprintf_P(bc + pos, sizeof(bc) - pos, PSTR(" > "));
    }
  }

  if (pos < sizeof(bc) - 1)
    bc[pos++] = ']';
  bc[pos] = '\0';

  /* Centre the breadcrumb in an 80‑char separator line */
  printSeparator(bc, '-', 64, 0, 0);
}

/* Remove the old breadcrumb function - using table-driven approach now */

void init() {
  // TRACE();
  menuDepth = 0;
  menuStack[0] = NodeID::MAIN;
}

// Menu display function table stored in PROGMEM
struct MenuDisplayHandler {
  NodeID menuId;
  void (*showFunction)();
};

const MenuDisplayHandler displayTable[] PROGMEM = {{NodeID::MAIN, showMain},
                                                   {NodeID::VIDEO, VideoTH::showMenu},
                                                   {NodeID::VIDEO_TESTS, VideoTH::showTests},
                                                   {NodeID::RAM, RamTH::showMenu},
                                                   {NodeID::RAM_TESTS, RamTH::showTestsMenu},
                                                   {NodeID::ROM, RomTH::showMenu},
                                                   {NodeID::KB, KeyboardTH::showMenu},
                                                   {NodeID::CAS, CassTH::showMenu},
                                                   {NodeID::CAS_SONGS, CassTH::showSongsMenu},
                                                   {NodeID::SYS_CONFIG, SysConfigTH::showMenu},
                                                   {NodeID::PORTS, PortsTH::showMenu}};

const uint8_t displayTableSize = sizeof(displayTable) / sizeof(MenuDisplayHandler);

void showCurrent() {
  // TRACE();
  clearScreen();
  showBreadcrumb();
  NodeID currentMenu = current();

  // Execute the appropriate display function
  for (uint8_t i = 0; i < displayTableSize; i++) {
    MenuDisplayHandler handler;
    memcpy_P(&handler, &displayTable[i], sizeof(MenuDisplayHandler));

    if (handler.menuId == currentMenu) {
      handler.showFunction();
      return;
    }
  }

  // Else fallback for unknown menu
  println(F("Unknown menu state"));
}

void pollSerial() {
  // TRACE();

  if (!Serial.available()) {
    // println("Serial port is empty");
    return;
  }

  // Debug: Check memory before processing input
  int freeMemory = getFreeMemory();
  if (freeMemory >= 0 && freeMemory < 800) {  // Lowered threshold to see improvements sooner
    print(F("[WARNING: Low memory in pollSerial: "));
    print(freeMemory);
    println(F(" bytes]"));
  }

  static char inputBuffer[64] = "";
  static uint8_t inputPos = 0;
  static bool lastWasNewline = false;
  char c = Serial.read();

  // Handle backspace
  if (c == '\b' || c == 127) {  // Backspace or DEL
    if (inputPos > 0) {
      inputPos--;
      inputBuffer[inputPos] = '\0';
      Serial.print(F("\b \b"));  // Erase character on terminal
    }
    lastWasNewline = false;
    return;
  }

  // Handle Enter key (line termination) - handle both \r and \n but avoid double processing
  if (c == '\r' || c == '\n') {
    // Skip if we just processed a newline character (avoid \r\n double processing)
    if (lastWasNewline) {
      lastWasNewline = false;
      return;
    }

    lastWasNewline = true;
    Serial.println();  // Echo newline

    if (inputPos == 0) {
      strcpy(inputBuffer, "?");  // Treat empty input as help request
    }

    // Process the complete line
    handleInputLine_safe(inputBuffer);

    // Clear buffer for next input
    inputBuffer[0] = '\0';
    inputPos = 0;
    return;
  }

  lastWasNewline = false;

  // Echo printable characters and add to buffer
  if (c >= ' ' && c <= '~' && inputPos < sizeof(inputBuffer) - 1) {
    Serial.print(c);
    inputBuffer[inputPos] = c;
    inputPos++;
    inputBuffer[inputPos] = '\0';  // Always null-terminate
  }
}

// Table-driven menu architecture for Arduino
struct MenuEntry {
  char key;
  NodeID targetMenu;
  void (*action)();
};

// Forward declaration for special actions
void exitProgram() {
  println(F("Bye!"));
  while (true)
    delay(1);
}

void showSystemConfig() {
  systemConfiguration();
}

// Main menu command table stored in PROGMEM
const MenuEntry mainMenuTable[] PROGMEM = {
    {'c', NodeID::CAS, nullptr},           {'x', NodeID::MAIN, exitProgram},  // Special action
    {'p', NodeID::PORTS, nullptr},         {'k', NodeID::KB, nullptr},
    {'m', NodeID::RAM, nullptr},           {'r', NodeID::ROM, nullptr},
    {'s', NodeID::MAIN, showSystemConfig},  // Special action, stay in main
    {'v', NodeID::VIDEO, nullptr}};

const uint8_t mainMenuTableSize = sizeof(mainMenuTable) / sizeof(MenuEntry);

// Module dispatch table stored in PROGMEM
struct ModuleHandler {
  NodeID menuId;
  void (*handler)(const char *);
};

const ModuleHandler moduleTable[] PROGMEM = {
    {NodeID::VIDEO, VideoTH::handleInput}, {NodeID::VIDEO_TESTS, VideoTH::handleTestsInput},
    {NodeID::RAM, RamTH::handleInput},     {NodeID::RAM_TESTS, RamTH::handleInput},
    {NodeID::ROM, RomTH::handleInput},     {NodeID::KB, KeyboardTH::handleInput},
    {NodeID::CAS, CassTH::handleInput},    {NodeID::CAS_SONGS, CassTH::handleInput},
    {NodeID::PORTS, PortsTH::handleInput}, {NodeID::SYS_CONFIG, SysConfigTH::handleInput}};

const uint8_t moduleTableSize = sizeof(moduleTable) / sizeof(ModuleHandler);

bool handleMainMenu(char c) {
  for (uint8_t i = 0; i < mainMenuTableSize; i++) {
    MenuEntry entry;
    memcpy_P(&entry, &mainMenuTable[i], sizeof(MenuEntry));

    if (entry.key == c) {
      if (entry.action) {
        // Execute special action
        entry.action();
      } else {
        // Navigate to target menu
        push(entry.targetMenu);
      }
      return true;
    }
  }
  return false;  // Command not found
}

bool handleModuleInput(NodeID currentMenu, const char *input) {
  for (uint8_t i = 0; i < moduleTableSize; i++) {
    ModuleHandler handler;
    memcpy_P(&handler, &moduleTable[i], sizeof(ModuleHandler));

    if (handler.menuId == currentMenu) {
      handler.handler(input);
      return true;
    }
  }
  return false;  // Module not found
}

void trimString_safe(const char *input, char *output, size_t outputSize) {
  if (!input || !output || outputSize == 0) {
    if (output && outputSize > 0)
      output[0] = '\0';
    return;
  }

  const char *start = input;
  const char *end = input + strlen(input) - 1;

  // Trim leading whitespace
  while (*start && isspace(*start)) {
    start++;
  }

  // Trim trailing whitespace
  while (end > start && isspace(*end)) {
    end--;
  }

  // Copy trimmed string
  size_t len = end - start + 1;
  if (len >= outputSize) {
    len = outputSize - 1;
  }

  strncpy(output, start, len);
  output[len] = '\0';
}

// Legacy String version - deprecated, use trimString_safe instead
// DISABLED: Uncomment only if you need to use this legacy function for debugging
#if 0
String trimString(const String& str) {
  String trimmed = str;

  // Trim leading whitespace (spaces, tabs, newlines, etc.)
  while (trimmed.length() > 0 && isspace(trimmed.charAt(0))) {
    trimmed.remove(0, 1);
  }

  // Trim trailing whitespace (spaces, tabs, newlines, etc.)
  while (trimmed.length() > 0 && isspace(trimmed.charAt(trimmed.length() - 1))) {
    trimmed.remove(trimmed.length() - 1, 1);
  }

  return trimmed;
}
#endif

void handleInputLine_safe(const char *input) {
  if (!input)
    return;

  // Trim whitespace from input
  static char trimmedInput[64];
  trimString_safe(input, trimmedInput, sizeof(trimmedInput));

  // Handle global commands first
  if (strlen(trimmedInput) == 1) {
    char c = trimmedInput[0];
    if (handleGlobalCommandBool(c)) {
      return;
    }

    // Handle escape
    if (c == 'x') {
      if (menuDepth > 0) {
        pop();
      } else {
        println(F("Bye!"));
        while (true)
          delay(1);
      }
      showCurrent();
      return;
    }
  }

  // Table-driven menu handling
  if (current() == NodeID::MAIN) {
    if (strlen(trimmedInput) == 1) {
      char c = trimmedInput[0];
      if (handleMainMenu(c)) {
        showCurrent();
        return;
      }
    }
    // Invalid input or help request
    showCurrent();
    return;
  }

  // Table-driven module input handling
  if (handleModuleInput(current(), trimmedInput)) {
    showCurrent();
    return;
  }

  // Fallback - show current menu
  showCurrent();
}

// Legacy String version - use handleInputLine_safe instead
// DISABLED: Uncomment only if you need to use this legacy function for debugging
#if 0
void handleInputLine(const String& input)
{
  // Convert to C-string and use safe version
  static char inputBuffer[64];
  input.toCharArray(inputBuffer, sizeof(inputBuffer));
  handleInputLine_safe(inputBuffer);
}
#endif

void clearScreen() {
  video.cls();
  lcdClear(ST77XX_BLACK);
}

void pollButtons() {
#if TEST_HARNESS_BUTTONS
  // char c = readShieldButton();
  char c = 5;
  if (c) {
    Serial.write(c);
  }
#endif
}

void triggerInterrupt() {
  println(F("[TODO] Triggering interrupt..."));
  // Implement interrupt triggering logic here
}
void toggleWaitSignal() {
  println(F("[TODO] Toggling WAIT signal..."));
  // Implement wait signal toggling logic here
}

void testSignal() {
  toggleTestSignal();
  print(F("TEST* signal toggled ["));
  // Switch: 0 = use Model1TH::getState() (mock), 1 = use Model1.getState() (real)
  bool isHigh;

  if (0) {
    // Use real Model1.getState()
    const char *currentState = Model1.getState();

    // Debug: Show the raw state string
    print(F("Raw state: "));
    println(currentState);

    // Debug: Show what parseSignalState returns
    const char *parsed = parseSignalState("TEST", currentState);
    if (parsed) {
      print(F("Parsed TEST: "));
      println(parsed);
    } else {
      println(F("Parse failed - TEST not found"));
    }

    isHigh = isSignalHigh("TEST", currentState);
  } else {
    // Use mock Model1TH::getState()
    isHigh = isSignalHigh("TEST", Model1TH::getState());
  }

  if (isHigh) {
    print(F("HIGH"));
  } else {
    print(F("LOW"));
  }

  println(F("]"));
}

}  // namespace Menu
