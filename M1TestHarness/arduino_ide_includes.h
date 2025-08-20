// Arduino IDE helper file to ensure all screen .cpp files are compiled
// This file should only be used when compiling with Arduino IDE
// PlatformIO doesn't need this as it handles subdirectories automatically

#ifdef ARDUINO_IDE_COMPILATION

// Include all screen implementation files - main level
#include "./screens/BoardRevisionMenu.cpp"
#include "./screens/DiagnosticConsole.cpp"
#include "./screens/HardwareDetectionConsole.cpp"
#include "./screens/MainMenu.cpp"
#include "./screens/RAMTestSuiteConsole.cpp"
#include "./screens/WelcomeConsole.cpp"

// About screens
#include "./screens/about/AboutConsole.cpp"

// Advanced screens
#include "./screens/advanced/AdvancedMenu.cpp"
#include "./screens/advanced/AdvancedSignalController.cpp"
#include "./screens/advanced/SignalGenerator.cpp"
#include "./screens/advanced/SignalOscilloscope.cpp"

// Cassette screens
#include "./screens/cassette/CassetteMenu.cpp"
#include "./screens/cassette/CassetteSongData.cpp"
#include "./screens/cassette/CassetteSongPlayerMenu.cpp"
#include "./screens/cassette/CassetteTestSuiteConsole.cpp"

// DRAM screens
#include "./screens/dram/DRAMContentViewerConsole.cpp"
#include "./screens/dram/DRAMMenu.cpp"
#include "./screens/dram/DRAMTestSuiteConsole.cpp"

// Keyboard screens
#include "./screens/keyboard/KeyboardTester.cpp"

// ROM screens
#include "./screens/rom/ROMContentViewerConsole.cpp"
#include "./screens/rom/ROMDetectionConsole.cpp"
#include "./screens/rom/ROMMenu.cpp"

// Video screens
#include "./screens/video/M1Terminal.cpp"
#include "./screens/video/VRAMContentViewerConsole.cpp"
#include "./screens/video/VRAMTestSuiteConsole.cpp"
#include "./screens/video/VideoMenu.cpp"
#include "./screens/video/test_screens/VideoTestScreensMenu.cpp"

#endif  // ARDUINO_IDE_COMPILATION
