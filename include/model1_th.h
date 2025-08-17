#ifndef MODEL1_TH_H
#define MODEL1_TH_H

#include <Arduino.h>
#include "globals_th.h"

// Memory page size constant
#define MEMORY_PAGE_SIZE 16

// Global memory buffer for read/write operations
extern uint8_t memoryBuffer[MEMORY_PAGE_SIZE];

// PROGMEM test patterns for memory testing
extern const uint8_t testPattern1[256] PROGMEM;
extern const uint8_t testPattern2[256] PROGMEM;

// Show system configuration menu or output
void systemConfiguration();

// Get memory size
uint16_t getMemorySize();

// Detect if lowercase mode exists
bool hasLowercaseInstalled();

// Initialize all major subsystems for the test harness
void initHarness(uint8_t refresh_timer = -1);

// Reset all major subsystems for the test harness
void resetHarness();

// Function to write data to memory and verify
void writeMemoryData(const uint8_t* data, size_t dataSize, uint16_t address, bool verify = false);

// Function to read a page of memory data and return pointer to buffer
uint8_t* readMemoryData(uint16_t address, uint16_t size = MEMORY_PAGE_SIZE);

// Function to get direct access to the global memory buffer
uint8_t* getMemoryBuffer();

// Function to print memory data from buffer (helper for readMemoryData)
void printMemoryData(const uint8_t* buffer, uint16_t address, uint16_t size);

// Function to copy PROGMEM pattern to RAM buffer (helper for PROGMEM patterns)
void copyProgmemPattern(const uint8_t* progmemPattern, uint8_t* ramBuffer, uint16_t size = MEMORY_PAGE_SIZE);

// Function to write PROGMEM pattern directly to memory
void writeProgmemPatternToMemory(const uint8_t* progmemPattern, uint16_t address, uint16_t size = MEMORY_PAGE_SIZE, bool verify = false);

// Toggle the TEST signal using Model1 only
void toggleTestSignal();

// Print each state from Model1.getState() in a single column, one per line, raw.
void printModel1State(const char *state);

// Given a signal name and a state string, return the human-readable state for that signal.
// Example: getSignalStateHuman("TEST", state) -> "Test Signal: 0"
// DEPRECATED: Use parseSignalState() instead to avoid memory leaks
// String getSignalStateHuman(const char *signal, const char *state);

// Parse state string to extract any signal state and return formatted result
// Returns: pointer to static buffer with formatted signal state, or NULL if not found
// Buffer format: "Signal Name: value" (max 25 chars)
const char* parseSignalState(const char *signal, const char *state);

// Helper function to check if a signal is HIGH (1) based on parsed result
// Returns: true if signal value is "1", false otherwise
bool isSignalHigh(const char *signal, const char *state);

// Return a human-readable form of a signal name (e.g., "TEST" -> "Test Signal").
const char *signalNameToHuman(const char *abbr);

// Print current TEST signal state in a human-readable format.
void printTestSignalState();

// Force cache invalidation when state changes (e.g., after toggling TEST signal)
// NOTE: Caching has been removed, but function kept for compatibility
void invalidateStateCache();


/**
 * Print a human-readable version of Model1.getState() output.
 * It is robust against overlong fields (ADDR, DATA, etc.) and will not
 * corrupt the stack if the model starts emitting wider bitfields.
 *
 * Example raw:
 *   Mut<T>, RfshEn<F>, RfshRow<  0>, RD<o>(1), WR<o>(1), IN<o>(1), OUT<o>(1),
 *   ADDR<o o-w>(00000000 00000000), DATA<i-r>(11111111), ...
 */
void printModel1StateNice(const char *state);

// Model1TH namespace for test harness specific functions
namespace Model1TH
{
    // Mock state function for testing - returns a fixed state string
    const char* getState();
}

void printFont();
void triggerInterrupt();
void toggleWaitSignal();

void waitSignal();
void testSignal();

#endif // MODEL1_TH_H
