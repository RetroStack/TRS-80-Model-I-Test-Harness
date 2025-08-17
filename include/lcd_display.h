#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

/* --------------------------------------------------------------
 *  Pin defaults -- override with -D flags or before this include
 * -------------------------------------------------------------- */
#ifndef LCD_TFT_CS
#define LCD_TFT_CS  9     // Chip-select
#endif
#ifndef LCD_TFT_DC
#define LCD_TFT_DC   8     // Data/Command
#endif
#ifndef LCD_TFT_RST
#define LCD_TFT_RST  -1     // Reset (-1 -> tied HIGH)
#endif

extern Adafruit_ST7789 lcd;          // concrete driver

bool lcdInit(uint8_t rotation = 3,    // Adafruit_GFX rotation 0-3
             uint16_t w = 240,        // panel width  (edit for 240x320 etc.)
             uint16_t h = 320);       // panel height

void lcdClear(uint16_t colour = ST77XX_BLACK);
inline void lcdSetCursor(int16_t x, int16_t y) { lcd.setCursor(x, y); }
inline void lcdUpdate() {}            // placeholder for future use

#endif /* LCD_DISPLAY_H */
