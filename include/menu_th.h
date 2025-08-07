#ifndef MENU_TH_H
#define MENU_TH_H

#include <Arduino.h>
#include <avr/pgmspace.h>

#include "global_utils_th.h"
#include "globals_th.h"
#include "utils_th.h"

// Forward declarations to avoid circular dependencies
namespace CassTH {
void showMenu();
void showSongsMenu();
void handleInput(const char* input);
}  // namespace CassTH
namespace KeyboardTH {
void showMenu();
void handleInput(const char* input);
}  // namespace KeyboardTH
namespace PortsTH {
void showMenu();
void handleInput(const char* input);
}  // namespace PortsTH
namespace RamTH {
void showMenu();
void showTestsMenu();
void handleInput(const char* input);
}  // namespace RamTH
namespace RomTH {
void showMenu();
void handleInput(const char* input);
}  // namespace RomTH
namespace SysConfigTH {
void showMenu();
void handleInput(const char* input);
}  // namespace SysConfigTH
namespace VideoTH {
void showMenu();
void showTests();
void handleInput(const char* input);
void handleTestsInput(const char* input);
}  // namespace VideoTH

namespace Menu {

constexpr int MAX_MENU_DEPTH = 5;

enum class NodeID {
  MAIN,
  VIDEO,
  VIDEO_TESTS,
  RAM,
  RAM_TESTS,  // Added for RAM tests submenu
  ROM,
  KB,
  CAS,
  CAS_SONGS,  // Added for cassette songs submenu
  SYS_CONFIG,
  PORTS
};

void init();
void showCurrent();
void pollSerial();
void pollButtons();
void push(NodeID node);
void pop();
NodeID current();
void showBreadcrumb();
void handleGlobalCommand(char c);
void handleInputLine(const String& input);     // Legacy - use handleInputLine_safe instead
void handleInputLine_safe(const char* input);  // Memory-safe version
String trimString(const String& str);          // Legacy - use trimString_safe instead
void trimString_safe(const char* input, char* output, size_t outputSize);  // Memory-safe version
void clearScreen();
void testSignal();
void waitSignal();
void exitProgram();
}  // namespace Menu

#endif  // MENU_TH_H
