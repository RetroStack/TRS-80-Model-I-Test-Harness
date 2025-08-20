# GitHub Copilot Instructions for TRS-80 Model I Test Harness

## Project Overview

This is a comprehensive test harness for the TRS-80 Model I computer, built using PlatformIO for Arduino Mega 2560. The project provides hardware-level testing, diagnostics, and emulation capabilities for vintage TRS-80 Model I systems.

## Architecture & Components

### Hardware Platform

- **Target**: Arduino Mega 2560
- **Display**: ST7789 TFT LCD (240x320 pixels)
- **Build System**: PlatformIO with Arduino framework
- **Language**: C++ (Arduino/embedded style)

### Core Modules

- **Model1 Library**: Hardware abstraction layer for TRS-80 Model I
- **Video System**: Terminal emulation and display management
- **Memory Testing**: RAM/ROM verification and diagnostic tools
- **Signal Testing**: Bus signal analysis and crosstalk detection
- **I/O Port Testing**: Cassette, keyboard, and peripheral interfaces
- **Test Harness Framework**: Menu-driven diagnostic interface

### File Structure

```
├── platformio.ini          # PlatformIO configuration
├── src/                     # Main application source
│   ├── main.cpp            # Entry point and setup
│   ├── MainScreen.cpp      # Primary user interface
│   ├── DiagnosticScreen.cpp # Hardware diagnostic displays
│   ├── WelcomeScreen.cpp   # Startup screen
│   └── globals.cpp         # Global state management
├── include/                 # Test harness modules
│   ├── *_th.cpp/h          # Test harness components
│   ├── lcd_display.cpp/h   # Display abstraction
│   └── utils_th.cpp/h      # Utility functions
├── M1TestHarness/          # Arduino sketch compatibility
└── lib/                    # External libraries
```

## Coding Standards & Conventions

### Code Style

- **Character Encoding**: ASCII ONLY - No Unicode characters allowed
- **Indentation**: 2 spaces, no tabs
- **Line Endings**: Unix (LF)
- **Function Names**: camelCase
- **Constants**: UPPER_SNAKE_CASE
- **Variables**: camelCase
- **Classes**: PascalCase

### Memory Management

- **Static Allocation Preferred**: Avoid dynamic allocation on Arduino
- **Buffer Sizes**: Use predefined constants (e.g., MEMORY_PAGE_SIZE = 256)
- **PROGMEM**: Store large constant data in flash memory
- **String Literals**: Use F() macro for flash storage: `println(F("Hello"))`

### Hardware Interaction Patterns

```cpp
// Model1 library usage
Model1.readMemory(address);
Model1.writeMemory(address, value);
Model1.getState();  // Returns bus state string

// Display output (dual LCD/TFT and serial)
print(TO_LCD, F("Message"));     // LCD only
println(TO_SERIAL, F("Debug"));  // Serial only
println(TO_BOTH, F("Info"));     // Both outputs
```

### Error Handling

- **Defensive Programming**: Always check pointers for NULL
- **State Validation**: Verify hardware state before operations
- **Timeout Patterns**: Use millis() for non-blocking timeouts
- **Error Reporting**: Clear, actionable error messages to user

## Key Technical Concepts

### Signal State Parsing

The system parses hardware state strings in format:

```
"RD<o>(1),WR<o>(1),ADDR<o o-w>(00111100),DATA<i-r>(00111001)"
```

- Signal name, direction in angle brackets, value in parentheses
- Use `parseSignalState()` and `isSignalHigh()` helpers

### Crosstalk Detection

Enhanced crosstalk matrix tracks signal interactions:

```cpp
struct CrosstalkResult {
  uint64_t crosstalkMatrix[36];  // Bit matrix for 36 signals
  uint16_t totalCrosstalkCount;
  // Detailed source-to-destination mapping
};
```

### Memory Testing Patterns

```cpp
// Memory verification with restore
uint8_t orig = Model1.readMemory(addr);
uint8_t test = ~orig;  // Invert pattern
Model1.writeMemory(addr, test);
bool ok = (Model1.readMemory(addr) == test);
Model1.writeMemory(addr, orig);  // Always restore
```

### Display Management

- **Dual Output**: TFT LCD for user interface, Serial for debugging
- **Screen Management**: Welcome → Main → Diagnostic screens
- **Text Formatting**: 80-character width with wrapping
- **Color Coding**: Use ST77XX color constants

## Common Patterns & Anti-Patterns

### ✅ Do This

```cpp
// Use static buffers to avoid stack allocation
static char buffer[128];

// Always restore hardware state after testing
uint8_t original = Model1.readMemory(addr);
// ... test code ...
Model1.writeMemory(addr, original);

// Use F() macro for string literals
println(TO_LCD, F("Status: OK"));

// Check return values and handle errors
const char* state = Model1.getState();
if (!state) {
  println(TO_LCD, F("ERROR: Failed to get state"));
  return;
}
```

### ❌ Avoid This

```cpp
// Don't use dynamic allocation
char* buffer = malloc(128);  // NO!

// Don't use Unicode characters
println("Status: ✓");  // NO! ASCII only

// Don't ignore error conditions
Model1.writeMemory(addr, value);  // Should check if successful

// Don't use naked string literals (wastes RAM)
println("Hello World");  // Should be F("Hello World")
```

## Testing & Debugging

### Unit Testing Approach

- **Hardware Mocking**: Use Model1TH namespace for mock implementations
- **State Verification**: Compare expected vs actual signal states
- **Memory Patterns**: Use known patterns (0x55, 0xAA, incremental)
- **Boundary Testing**: Test memory limits and edge cases

### Debug Output

```cpp
// Debug patterns for development
#ifdef DEBUG_MODE
  println(TO_SERIAL, F("Debug: Signal state = "), signalState);
#endif

// Production error handling
if (error) {
  println(TO_LCD, F("ERROR: "), errorMessage);
  inputPrompt(TO_LCD, F("Press any key to continue..."));
}
```

### Signal Analysis

- **Bus State Monitoring**: Real-time signal level display
- **Crosstalk Detection**: Matrix-based interference analysis
- **Timing Analysis**: Signal transition monitoring
- **Protocol Verification**: Memory/IO operation validation

## Integration Guidelines

### Model1 Library Integration

- **State Management**: Use getState() for real-time bus monitoring
- **Memory Operations**: Prefer readMemory/writeMemory over direct access
- **Signal Control**: Use library functions for TEST, WAIT, INT signals
- **Hardware Abstraction**: Don't bypass library for hardware access

### Display Integration

- **Screen Navigation**: Maintain consistent menu structure
- **User Feedback**: Provide clear progress indicators
- **Error Display**: User-friendly error messages with recovery options
- **Status Reporting**: Real-time hardware status updates

### Performance Considerations

- **Minimize Memory Allocation**: Use static/global buffers
- **Optimize Display Updates**: Batch screen operations when possible
- **Reduce String Operations**: Minimize dynamic string manipulation
- **Hardware Access Patterns**: Batch read/write operations efficiently

## Special Considerations

### ASCII-Only Requirement

- **Strict ASCII**: All source files must contain only ASCII characters (0x00-0x7F)
- **No Unicode**: Replace any Unicode with ASCII equivalents
- **Character Validation**: Use `grep -P '[^\x00-\x7F]'` to verify compliance
- **Common Replacements**:
  - Smart quotes → ASCII quotes `"`
  - Em/en dashes → ASCII hyphens `-`
  - Approximately equal `≈` → ASCII tilde `~`

### Hardware Compatibility

- **Signal Timing**: Respect TRS-80 Model I timing requirements
- **Voltage Levels**: Ensure 5V TTL compatibility
- **Bus Loading**: Minimize electrical impact on original hardware
- **Safety**: Always restore original state after testing

### Error Recovery

- **Graceful Degradation**: Continue operation when possible
- **State Restoration**: Always restore hardware to safe state
- **User Guidance**: Provide clear recovery instructions
- **System Reset**: Implement emergency reset procedures

## Documentation Standards

- **Function Comments**: Describe purpose, parameters, return values
- **Code Comments**: Explain complex logic and hardware interactions
- **User Messages**: Clear, actionable text for operators
- **Error Messages**: Specific problem description with suggested action

Remember: This is embedded systems code for vintage hardware testing. Prioritize reliability, safety, and clear user feedback over advanced language features.
