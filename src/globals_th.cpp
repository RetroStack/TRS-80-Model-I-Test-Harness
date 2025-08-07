#include "globals_th.h"

// Define global instances
Cassette cassette;
Keyboard keyboard;
ROM rom;
SerialLogger logger;
Video video;

// Define static members of th
size_t th::g_ramSize = 0;          // RAM size in bytes
bool th::g_echoToDisplay = false;  // Flag to echo output to display

// Helper function to check if input matches a command (1 or 2 characters)
bool isCommand(const char* input, const char* command) {
  if (!input || !command)
    return false;

  size_t inputLen = strlen(input);
  size_t cmdLen = strlen(command);

  // Only support 1 or 2 character commands
  if (cmdLen > 2 || inputLen != cmdLen)
    return false;

  return strcmp(input, command) == 0;
}
