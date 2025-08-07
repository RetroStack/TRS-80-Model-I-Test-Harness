#ifndef UTILS_TH_H
#define UTILS_TH_H

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Video.h>
#include <avr/pgmspace.h>

/* ----------------------------------------------------------------
 *  Constants and Buffer Configuration
 * -------------------------------------------------------------- */
#define MAX_BUFFER_SIZE 256
#define MAX_INPUT_PARAMETERS 5

/* ----------------------------------------------------------------
 *  Display Type Declarations (Forward Declarations)
 * -------------------------------------------------------------- */
struct LCDDisplay {
  bool state;
};

struct M1Display {
  bool state;
};

constexpr LCDDisplay TO_LCD{true};
constexpr M1Display TO_M1{true};

/* ----------------------------------------------------------------
 *  Global Variables
 * -------------------------------------------------------------- */
extern char inputBuffer[MAX_BUFFER_SIZE];
extern Adafruit_GFX *gfx;  // Global pointer for graphics display
extern Video video;

/* ----------------------------------------------------------------
 *  Serial Input/Output Functions
 * -------------------------------------------------------------- */
void serialFlush();
uint8_t inputPrompt(const char *str);
uint8_t inputPrompt(const __FlashStringHelper *str);
uint8_t inputPrompt(const M1Display &display, const char *str);
uint8_t inputPrompt(const M1Display &display, const __FlashStringHelper *str);
uint8_t inputPrompt(const LCDDisplay &display, const char *str);
uint8_t inputPrompt(const LCDDisplay &display, const __FlashStringHelper *str);
uint8_t readSerialInputParse(char *outParams[], uint8_t maxParams);

/* ----------------------------------------------------------------
 *  String Utilities
 * -------------------------------------------------------------- */
uint16_t parseNumber(const char *str);
uint16_t strToUint16(const char *str);
void removeSpaces(char *str);
void removeWhitespaces(char *str);

/* ----------------------------------------------------------------
 *  Print Functions
 * -------------------------------------------------------------- */
void printBinary(uint16_t n, uint8_t width);

/* ----------------------------------------------------------------
 *  Display Support
 * -------------------------------------------------------------- */
inline void setDisplay(Adafruit_GFX &d) {
  gfx = &d;
}

/* ----------------------------------------------------------------
 *  Format Tag Types
 * -------------------------------------------------------------- */
struct HexFormatType {};
struct OctFormatType {};
struct DecFormatType {};
struct BinFormatType {};

constexpr HexFormatType Hex;
constexpr OctFormatType Oct;
constexpr DecFormatType Dec;
constexpr BinFormatType Bin;

/* ----------------------------------------------------------------
 *  Print Functions - Base Cases
 * -------------------------------------------------------------- */
inline void println() {
  Serial.println();
}
inline void print() {}  // no‑op end of recursion

/* ----------------------------------------------------------------
 *  Print Functions - Generic Variadic Helpers
 * -------------------------------------------------------------- */
template <typename T, typename... Rest>
void println(T first, Rest... rest) {
  Serial.print(first);
  println(rest...);
}

template <typename T, typename... Rest>
void print(T first, Rest... rest) {
  Serial.print(first);
  print(rest...);
}

/* ----------------------------------------------------------------
 *  Print Functions - Format-Aware Overloads
 * -------------------------------------------------------------- */
template <typename T, typename... Rest>
void print(T value, BinFormatType, Rest... rest) {
  Serial.print(value, BIN);
  print(rest...);
}
template <typename T, typename... Rest>
void println(T value, BinFormatType, Rest... rest) {
  Serial.print(value, BIN);
  println(rest...);
}

template <typename T, typename... Rest>
void print(T value, HexFormatType, Rest... rest) {
  Serial.print(value, HEX);
  print(rest...);
}
template <typename T, typename... Rest>
void println(T value, HexFormatType, Rest... rest) {
  Serial.print(value, HEX);
  println(rest...);
}

template <typename T, typename... Rest>
void print(T value, OctFormatType, Rest... rest) {
  Serial.print(value, OCT);
  print(rest...);
}
template <typename T, typename... Rest>
void println(T value, OctFormatType, Rest... rest) {
  Serial.print(value, OCT);
  println(rest...);
}

template <typename T, typename... Rest>
void print(T value, DecFormatType, Rest... rest) {
  Serial.print(value, DEC);
  print(rest...);
}
template <typename T, typename... Rest>
void println(T value, DecFormatType, Rest... rest) {
  Serial.print(value, DEC);
  println(rest...);
}

/* ----------------------------------------------------------------
 *  LCD Mirroring Support
 * -------------------------------------------------------------- */

// Send one argument to LCD
template <typename T>
void gfxPrint(const T &v) {
  if (gfx)
    gfx->print(v);
}
inline void gfxPrint(HexFormatType) {}  // ignore tags
inline void gfxPrint(OctFormatType) {}
inline void gfxPrint(DecFormatType) {}

inline void gfxPrintMany() {}

template <typename T, typename... Rest>
void gfxPrintMany(const T &first, Rest... rest) {
  gfxPrint(first);
  gfxPrintMany(rest...);
}

template <typename... Rest>
void print(LCDDisplay, Rest... rest) {
  gfxPrintMany(rest...);
  print(rest...);
}

template <typename... Rest>
void println(LCDDisplay, Rest... rest) {
  gfxPrintMany(rest...);
  if (gfx)
    gfx->println();
  println(rest...);
}

// Helper functions for M1Display routing
inline void printToVideo() {
  // Base case - no more arguments
}

template <typename T>
void printToVideo(T first) {
  video.print(first);
}

template <typename T, typename... Rest>
void printToVideo(T first, Rest... rest) {
  video.print(first);
  printToVideo(rest...);
}

// Simplified M1Display functions - test with explicit overloads
inline void print(M1Display, const char *str) {
  video.print(str);
  Serial.print(str);
}

inline void print(M1Display, const __FlashStringHelper *str) {
  video.print(str);
  Serial.print(str);
}

inline void println(M1Display, const char *str) {
  video.println(str);
  Serial.println(str);
}

inline void println(M1Display, const __FlashStringHelper *str) {
  video.println(str);
  Serial.println(str);
}

// Variadic versions for multiple arguments
template <typename... Rest>
void print(M1Display, Rest... rest) {
  printToVideo(rest...);
  print(rest...);
}

template <typename... Rest>
void println(M1Display, Rest... rest) {
  printToVideo(rest...);
  video.println();
  println(rest...);
}

/* ----------------------------------------------------------------
 *  Formatting and Debug Utilities
 * -------------------------------------------------------------- */
void printSeparator(const char *label, char sep = '-', uint8_t width = 80, int8_t alignment = 0,
                    uint8_t leading = 0);

// Inline overload for flash strings
inline void printSeparator(const __FlashStringHelper *label, char sep = '-', uint8_t width = 80,
                           int8_t alignment = 0, uint8_t leading = 0) {
  char buffer[64];
  strcpy_P(buffer, (const char *)label);
  printSeparator(buffer, sep, width, alignment, leading);
}

// M1Display overloads for display routing
void printSeparator(M1Display display, const char *label, char sep = '-', uint8_t width = 80,
                    int8_t alignment = 0, uint8_t leading = 0);

inline void printSeparator(M1Display display, const __FlashStringHelper *label, char sep = '-',
                           uint8_t width = 80, int8_t alignment = 0, uint8_t leading = 0) {
  char buffer[64];
  strcpy_P(buffer, (const char *)label);
  printSeparator(display, buffer, sep, width, alignment, leading);
}

// LCDDisplay overloads for display routing
void printSeparator(LCDDisplay display, const char *label, char sep = '-', uint8_t width = 80,
                    int8_t alignment = 0, uint8_t leading = 0);

inline void printSeparator(LCDDisplay display, const __FlashStringHelper *label, char sep = '-',
                           uint8_t width = 80, int8_t alignment = 0, uint8_t leading = 0) {
  char buffer[64];
  strcpy_P(buffer, (const char *)label);
  printSeparator(display, buffer, sep, width, alignment, leading);
}

/* Trace macro */
#define TRACE()                   \
  do {                            \
    print(F("[TRACE] "));         \
    println(__PRETTY_FUNCTION__); \
  } while (0)

#endif /* UTILS_TH_H */
