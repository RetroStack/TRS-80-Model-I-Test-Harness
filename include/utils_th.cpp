#include "utils_th.h"

#include <avr/pgmspace.h>
#include <ctype.h>
#include <stdlib.h>

// Adafruit_GFX *gfx = nullptr; // Global pointer for graphics display

// Definitions for extern variables
char inputBuffer[MAX_BUFFER_SIZE] = {0};
char *parameters[MAX_INPUT_PARAMETERS] = {nullptr};

void println(const __FlashStringHelper *str) {
  Serial.println(str);
}

void print(const __FlashStringHelper *str) {
  Serial.print(str);
}

void serialFlush() {
  while (Serial.available()) {
    Serial.read();
  }
}

// Internal handler for all inputPrompt overloads
static uint8_t inputPrompt_internal(const char *str) {
  Serial.print(str);
  while (Serial.available() == 0) {
    // Wait for user input
  }
  return Serial.read();
}

// Input prompt function for C-style strings prompt msg
uint8_t inputPrompt(const char *str) {
  serialFlush();
  return inputPrompt_internal(str);
}

// Input prompt function for __FlashStringHelper strings prompt msg
uint8_t inputPrompt(const __FlashStringHelper *str) {
  static char buf[128];  // Make static to avoid stack allocation

  serialFlush();
  strcpy_P(buf, (const char *)str);
  return inputPrompt_internal(buf);
}

// Input prompt function for M1Display with C-style strings prompt msg
uint8_t inputPrompt(const M1Display &display, const char *str) {
  // For now, just delegate to the regular inputPrompt
  // The display parameter can be used for future display routing
  return inputPrompt(str);
}

// Input prompt function for M1Display with __FlashStringHelper strings prompt msg
uint8_t inputPrompt(const M1Display &display, const __FlashStringHelper *str) {
  // For now, just delegate to the regular inputPrompt
  // The display parameter can be used for future display routing
  return inputPrompt(str);
}

// Input prompt function for LCDDisplay with C-style strings prompt msg
uint8_t inputPrompt(const LCDDisplay &display, const char *str) {
  // For now, just delegate to the regular inputPrompt
  // The display parameter can be used for future display routing
  return inputPrompt(str);
}

// Input prompt function for LCDDisplay with __FlashStringHelper strings prompt msg
uint8_t inputPrompt(const LCDDisplay &display, const __FlashStringHelper *str) {
  // For now, just delegate to the regular inputPrompt
  // The display parameter can be used for future display routing
  return inputPrompt(str);
}

/**
 *  strToUint16()
 *  --------------------------------------------------------------
 *  Accepts numbers in these forms (with whitespace allowed anywhere):
 *    1234              <- decimal
 *    0x3C00 / 0X3C00   <- hexadecimal
 *    3C00h / 3C00H     <- hexadecimal with 'h' suffix
 *    0b11001100 / 0B11001100 <- binary
 *
 *  All whitespace characters are stripped before parsing,
 *  so strings like "  0x3C00  " or "3C 00 h" are handled.
 */

/**  Safe, non-destructive string->uint16 converter  */
uint16_t strToUint16(const char *src) {
  /* ---- 1.  build a clean, compact copy ---- */
  char buf[12];  // big enough for "0xFFFF\0"
  uint8_t i = 0;
  while (*src && i < sizeof(buf) - 1) {
    if (!isspace((uint8_t)*src))  // drop all white-space
      buf[i++] = *src;
    ++src;
  }
  buf[i] = '\0';

  /* ---- 2.  detect radix by prefix / suffix ---- */
  uint8_t len = i;
  uint8_t base = 10;

  if (len > 1) {
    char last = buf[len - 1];
    if (last == 'h' || last == 'H') {
      buf[len - 1] = '\0';
      base = 16;
    } else if (last == 'b' || last == 'B') {
      buf[len - 1] = '\0';
      base = 2;
    }
  }
  if (len > 2 && buf[0] == '0') {
    if (buf[1] == 'x' || buf[1] == 'X')
      base = 16;
    else if (buf[1] == 'b' || buf[1] == 'B')
      base = 2;
  }

  /* ---- 3.  convert ---- */
  return (uint16_t)strtol(buf, nullptr, base);
}

/**
 * printSeparator()
 * ------------------------------------------------------------------
 * Prints a decorated separator line
 *
 *   - label     : C-string to output (no surrounding spaces added).
 *   - sep       : single filler character (default '-').
 *   - width     : total visible width (default 80).
 *                 - width == 0 -> no automatic padding after label.
 *   - alignment : -1 = flush-left, 0 = centred, 1 = flush-right.
 *                 Ignored if leading > 0.
 *   - leading   : exact # of leading seps. When >0, alignment is ignored.
 *
 * SRAM-friendly: streams bytes directly, no large buffers.
 */
void printSeparator(const char *label, char sep, uint8_t width, int8_t alignment, uint8_t leading) {
  uint8_t len = strlen(label);  // label length 0-255

  /* ----- explicit leading mode ----- */
  if (leading > 0) {
    /* print the requested leading seps */
    for (uint8_t i = 0; i < leading; ++i)
      print(sep);

    /* print the label */
    print(label);

    /* trailing fill only if width > 0 */
    if (width > 0) {
      /* ensure width can accommodate everything */
      uint8_t used = leading + len;
      if (width < used)
        width = used;
      uint8_t trailing = width - used;
      while (trailing--)
        print(sep);
    }
    println();
    return;
  }

  /* ---- alignment modes (leading == 0) ---- */
  switch (alignment) {
    /* flush-left: label first, then trailing filler (if any) */
    case -1: {
      print(label);
      if (width > 0 && width > len) {
        uint8_t trailing = width - len;
        while (trailing--)
          print(sep);
      }
      println();
      return;
    }

    /* centred */
    case 0: {
      if (width == 0 || width < len)
        width = len;
      uint8_t rem = width - len;
      uint8_t left = rem / 2;
      uint8_t right = rem - left;
      while (left--)
        print(sep);
      print(label);
      while (right--)
        print(sep);
      println();
      return;
    }

    /* flush-right: all padding before label */
    case 1:
    default: {
      if (width == 0 || width < len) {
        /* width too small or disabled -> no padding */
        print(label);
        println();
        return;
      }
      uint8_t pad = width - len;  // all before label
      while (pad--)
        print(sep);
      print(label);
      println();
      return;
    }
  }
}

/**
 * printSeparator()
 * ------------------------------------------------------------------
 * Sends a separator line to both outputs.
 * - Video (TRS-80) width is clamped to 64 columns if width > 64.
 * - Serial/host respects the caller-supplied width exactly.
 * - width == 0 => no padding (only the label is printed).
 * - alignment: -1 = left, 0 = center, 1 = right (ignored if leading > 0).
 * - leading > 0 => print exactly 'leading' seps before the label and
 *                  then fill to the requested width (per-output).
 */
void printSeparator(M1Display display, const char *label, char sep, uint8_t width, int8_t alignment,
                    uint8_t leading) {
  const uint8_t len = (uint8_t)strlen(label);

  // Per-output widths
  const uint8_t ws = width;                      // serial/host width (unchanged)
  const uint8_t wv = (width > 64) ? 64 : width;  // video width (clamped at 64)

  auto put_video_seps = [&](uint8_t n) {
    while (n--)
      video.print(sep);
  };
  auto put_serial_seps = [&](uint8_t n) {
    while (n--)
      print(sep);
  };

  auto put_label = [&]() {
    // Video gets mapped glyphs; serial prints raw label
    for (const char *c = label; *c; ++c)
      video.print(video.convertLocalCharacterToModel1(*c));
    print(label);
  };

  auto end_lines = [&]() {
    video.println();
    println();
  };

  /* ---------- explicit leading mode ---------- */
  if (leading > 0) {
    // Emit requested leading seps (video is clamped)
    put_video_seps(leading > 64 ? 64 : leading);
    put_serial_seps(leading);

    put_label();

    if (ws > 0) {
      // Pad each output independently to its own target width
      const uint16_t used = (uint16_t)leading + len;

      const uint8_t target_v = (wv < used) ? (uint8_t)used : wv;
      const uint8_t target_s = (ws < used) ? (uint8_t)used : ws;

      if (target_v > used)
        put_video_seps(target_v - used);
      if (target_s > used)
        put_serial_seps(target_s - used);
    }
    end_lines();
    return;
  }

  /* ---------- alignment modes (leading == 0) ---------- */
  switch (alignment) {
    /* flush-left */
    case -1: {
      put_label();

      if (ws > 0) {
        // trailing padding to each width (if any)
        if (wv > len)
          put_video_seps(wv - len);
        if (ws > len)
          put_serial_seps(ws - len);
      }
      end_lines();
      return;
    }

    /* centered */
    case 0: {
      // For width == 0, treat as exactly the label (no padding)
      const uint8_t span_v = (wv == 0 || wv < len) ? len : wv;
      const uint8_t span_s = (ws == 0 || ws < len) ? len : ws;

      const uint8_t lv = (uint8_t)((span_v - len) / 2);
      const uint8_t rv = (uint8_t)(span_v - len - lv);

      const uint8_t ls = (uint8_t)((span_s - len) / 2);
      const uint8_t rs = (uint8_t)(span_s - len - ls);

      if (lv)
        put_video_seps(lv);
      if (ls)
        put_serial_seps(ls);

      put_label();

      if (rv)
        put_video_seps(rv);
      if (rs)
        put_serial_seps(rs);

      end_lines();
      return;
    }

    /* flush-right (default) */
    case 1:
    default: {
      // pre-pad then label (only if width allows)
      const uint8_t pv = (wv == 0 || wv < len) ? 0 : (wv - len);
      const uint8_t ps = (ws == 0 || ws < len) ? 0 : (ws - len);

      if (pv)
        put_video_seps(pv);
      if (ps)
        put_serial_seps(ps);

      put_label();
      end_lines();
      return;
    }
  }
}

/**
 * printSeparator() - LCDDisplay overload
 * ------------------------------------------------------------------
 * Sends a separator line to both LCD and Serial outputs.
 * - LCD output uses gfx->print() if gfx is available
 * - Serial output uses the regular print() functions
 * - width == 0 => no padding (only the label is printed)
 * - alignment: -1 = left, 0 = center, 1 = right (ignored if leading > 0)
 * - leading > 0 => print exactly 'leading' seps before the label and
 *                  then fill to the requested width
 */
void printSeparator(LCDDisplay display, const char *label, char sep, uint8_t width,
                    int8_t alignment, uint8_t leading) {
  const uint8_t len = (uint8_t)strlen(label);

  auto put_lcd_seps = [&](uint8_t n) {
    while (n--) {
      if (gfx)
        gfx->print(sep);
    }
  };

  auto put_serial_seps = [&](uint8_t n) {
    while (n--)
      print(sep);
  };

  auto put_label = [&]() {
    if (gfx)
      gfx->print(label);
    print(label);
  };

  auto end_lines = [&]() {
    if (gfx)
      gfx->println();
    println();
  };

  /* ---------- explicit leading mode ---------- */
  if (leading > 0) {
    put_lcd_seps(leading);
    put_serial_seps(leading);

    put_label();

    if (width > 0) {
      const uint16_t used = (uint16_t)leading + len;
      const uint8_t target = (width < used) ? (uint8_t)used : width;

      if (target > used) {
        put_lcd_seps(target - used);
        put_serial_seps(target - used);
      }
    }
    end_lines();
    return;
  }

  /* ---------- alignment modes (leading == 0) ---------- */
  switch (alignment) {
    /* flush-left */
    case -1: {
      put_label();

      if (width > 0 && width > len) {
        put_lcd_seps(width - len);
        put_serial_seps(width - len);
      }
      end_lines();
      return;
    }

    /* centered */
    case 0: {
      const uint8_t span = (width == 0 || width < len) ? len : width;
      const uint8_t left_pad = (uint8_t)((span - len) / 2);
      const uint8_t right_pad = (uint8_t)(span - len - left_pad);

      if (left_pad) {
        put_lcd_seps(left_pad);
        put_serial_seps(left_pad);
      }

      put_label();

      if (right_pad) {
        put_lcd_seps(right_pad);
        put_serial_seps(right_pad);
      }

      end_lines();
      return;
    }

    /* flush-right (default) */
    case 1:
    default: {
      const uint8_t pad = (width == 0 || width < len) ? 0 : (width - len);

      if (pad) {
        put_lcd_seps(pad);
        put_serial_seps(pad);
      }

      put_label();
      end_lines();
      return;
    }
  }
}

/* ------------------------------------------------------------------
 *  removeSpaces()
 *  -----------------------------------------------------------------
 *  - str must point to a writable, NUL-terminated char array
 *    (don't pass a string literal stored in flash/PROGMEM).
 *  - All space characters are stripped and the string is re-terminated.
 *  - Works in O(n) time, 0 B extra RAM.
 */
void removeSpaces(char *str) {
  char *src = str; /* read pointer  */
  char *dst = str; /* write pointer */

  while (*src) {
    if (*src != ' ') { /* faster: test literal */
      *dst++ = *src;   /* keep this char        */
    }
    ++src;
  }
  *dst = '\0'; /* re-terminate */
}

// Strip all whitespace characters (spaces, tabs, newlines, etc.)
// This function modifies the input string in place.
void removeWhitespaces(char *str) {
  char *src = str, *dst = str;
  while (*src) {
    if (!isspace((uint8_t)*src))  // tabs, CR/LF, etc. removed
      *dst++ = *src;
    ++src;
  }
  *dst = '\0';
}

/**
 *  readSerialInputParse()
 *  --------------------------------------------------------------
 *  Blocks until the user types a line terminated by CR / LF,
 *  tokenises the line on commas, and stores up to  maxParams
 *  pointers in  outParams[]. Doesn't remove whitespaces.
 *
 *  @param  outParams   Caller-allocated array of char*.
 *  @param  maxParams   Size of that array (usually MAX_INPUT_PARAMETERS).
 *  @return             Number of tokens actually stored (0-maxParams).
 */

/**  Reads a line, strips all white-space, splits on ",",
 *   and returns the number of tokens copied into out[]          */
uint8_t readSerialInputParse(char *out[], uint8_t max) {
  /* -------- read a full line into inputBuffer -------- */
  uint16_t idx = 0;
  serialFlush();  // clear any previous input
  while (true) {
    if (Serial.available()) {
      char c = Serial.read();

      if (c == '\n' || c == '\r') {
        Serial.write('\n');
        inputBuffer[idx] = '\0';
        break;
      }

      Serial.write(c);
      if (idx < MAX_BUFFER_SIZE - 1)
        inputBuffer[idx++] = c;
    }
  }

  /* -------- compact white-space once, globally -------- */
  removeWhitespaces(inputBuffer);

  /* -------- split on commas -------- */
  uint8_t n = 0;
  for (char *tok = strtok(inputBuffer, ","); tok && n < max; tok = strtok(nullptr, ",")) {
    if (*tok)
      out[n++] = tok;
  }
  return n;
}

// Print out number in padded binary format
void printBinary(uint16_t n, uint8_t width) {
  for (int8_t i = width - 1; i >= 0; --i) {
    uint8_t bit = (n >> i) & 1;
    print(bit, Bin);
  }
  return;
}
