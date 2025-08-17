#include <Arduino.h>

#include "./globals.h"
#include "./screens/WelcomeConsole.h"

// First, tell the system which display you have
// For ST7789 240x320 displays (most common, landscape becomes 320x240)
#include <Display_ST7789_320x240.h>
Display_ST7789_320x240 displayProvider;

// Compact logo (64x64, 16-color palette + RLE) in PROGMEM
#include "retro_stack_64x64_rle.h"

// For ST7789 240x240 square displays
// #include <Display_ST7789_240x240.h>
// Display_ST7789_240x240 displayProvider;

// For ST7789 320x170 wide displays (landscape)
// #include <Display_ST7789_320x170.h>
// Display_ST7789_320x170 displayProvider;

// For ST7789 320x240 alternative displays (landscape)
// #include <Display_ST7789_320x240.h>
// Display_ST7789_320x240 displayProvider;

// For smaller ST7735 128x160 displays
// #include <Display_ST7735.h>
// Display_ST7735 displayProvider;

// For large ST7796 320x480 displays (landscape becomes 480x320)
// #include <Display_ST7796.h>
// Display_ST7796 displayProvider;

// For parallel ILI9325 240x320 displays (landscape becomes 320x240)
// #include <Display_ILI9325.h>
// Display_ILI9325 displayProvider;

// For ILI9341 240x320 displays (landscape becomes 320x240)
// #include <Display_ILI9341.h>
// Display_ILI9341 displayProvider;

// For large HX8357 320x480 displays
// #include <Display_HX8357.h>
// Display_HX8357 displayProvider;

// For monochrome SSD1306 OLED displays (128x64)
// #include <Display_SSD1306.h>
// Display_SSD1306 displayProvider;

// For monochrome SH1106 OLED displays (128x64)
// #include <Display_SH1106.h>
// Display_SH1106 displayProvider;

// ---------- Small helpers ----------
// 16-bit read from PROGMEM (works on AVR; on non-AVR pgm_read_word usually maps to regular read)
static inline uint16_t pgm_read_u16(const void *addr) {
  return pgm_read_word(addr);
}

// Return the output height (in pixels) for source row r in [0..srcH-1],
// scaled to total dstH using pure integer math.
// This yields 3 or 4 for 320x240 from 64.
static uint8_t scaledRowHeight(uint16_t r, uint16_t srcH, uint16_t dstH) {
  uint16_t prev = (uint32_t)r * dstH / srcH;
  uint16_t next = (uint32_t)(r + 1) * dstH / srcH;
  return (uint8_t)(next - prev);  // 3 or 4 for 240/64
}

// Draw 64x64 paletted+RLE image scaled to exactly dstW x dstH,
// using filled rectangles (fast & blocky).
static void drawRLEPalettedScaledFill(Adafruit_GFX &gfx, int16_t x0, int16_t y0, uint16_t dstW,
                                      uint16_t dstH) {
  const uint16_t srcW = RETRO_RLE_WIDTH;   // 64
  const uint16_t srcH = RETRO_RLE_HEIGHT;  // 64
  const uint8_t scaleX = dstW / srcW;      // = 5 when 320x240
  const uint8_t scaleY_min = dstH / srcH;  // = 3 (but we vary between 3 and 4)

  (void)scaleY_min;  // only to quiet warnings if unused

  const uint8_t *p = retro_stack_64x64_rle_data;

  // Track source-space position to decide when we wrap rows.
  uint16_t srcX = 0;
  uint16_t srcY = 0;

  // Current destination cursor
  int16_t curX = x0;
  int16_t curY = y0;

  // Height of the current output row (3 or 4)
  uint8_t rowH = scaledRowHeight(srcY, srcH, dstH);

  // Process stream until we render all source pixels.
  const uint32_t totalSrc = (uint32_t)srcW * srcH;
  uint32_t drawnSrc = 0;

  while (drawnSrc < totalSrc) {
    uint8_t b = pgm_read_byte(p++);
    uint8_t len = (b >> 4) + 1;  // 1..16 pixels in source
    uint8_t ci = (b & 0x0F);     // palette index
    uint16_t color = pgm_read_u16(&retro_stack_64x64_rle_palette[ci]);

    while (len--) {
      // Width in destination pixels for this single source pixel
      uint16_t runW = scaleX;  // = 5 when 320 wide

      // Emit one filled rectangle for this single "pixel" in the run
      gfx.fillRect(curX, curY, runW, rowH, color);

      // Advance
      curX += runW;
      srcX++;
      drawnSrc++;

      // End of source row? wrap
      if (srcX >= srcW) {
        srcX = 0;
        srcY++;

        // advance destination Y by the row height and compute next row’s height
        curY += rowH;
        rowH = (srcY < srcH) ? scaledRowHeight(srcY, srcH, dstH) : 0;
        curX = x0;
      }
    }
  }
}

// --- your logo screen, full-screen scale with rectangles ---
void drawLogo(Adafruit_GFX &gfx, uint16_t width, uint16_t height) {
  gfx.fillScreen(0x000F);  // deep navy background

  // Scale the 64x64 source to exactly fill the display area.
  // For 320x240 this becomes 320x240 (5x horizontally, 3/4 vertically mixed).
  const int16_t x = 0;
  const int16_t y = 0;
  drawRLEPalettedScaledFill(gfx, x, y, width, height);
}

void setup() {
  Serial.begin(115200);

  Model1.begin(-1);  // Setup Timer 2
  M1Shield.begin(displayProvider);

  // Optionally enable joystick input for menus
  // M1Shield.activateJoystick();

  Globals.logger.infoF(F("=== TRS-80 Model 1 Testharness ==="));

  // Show logo
  drawLogo(displayProvider.getGFX(), displayProvider.width(), displayProvider.height());
  delay(5000);

  // Start with the welcome console
  M1Shield.setScreen(new WelcomeConsole());
}

ISR(TIMER2_COMPA_vect) {
  Model1.nextUpdate();
}

void loop() {
  M1Shield.loop();
}
