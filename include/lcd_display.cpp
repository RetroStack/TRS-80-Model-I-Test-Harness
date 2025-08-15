#include "lcd_display.h"
#include "utils_th.h"        // brings in extern Adafruit_GFX *gfx

/* --------------------------------------------------------------
 *  Global objects
 * -------------------------------------------------------------- */
Adafruit_ST7789 lcd(LCD_TFT_CS, LCD_TFT_DC, LCD_TFT_RST);
Adafruit_GFX   *gfx = nullptr;        // single definition for utils_th

/* -------------------------------------------------------------- */
static void setTextDefaults()
{
  lcd.setTextWrap(true);
  lcd.setTextSize(1);                 // 6×8 default font
  lcd.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  lcd.setCursor(0, 0);
}

bool lcdInit(uint8_t rotation, uint16_t w, uint16_t h)
{
  lcd.init(w, h);                     // power‑up the ST7789
  lcd.setRotation(rotation);
  lcd.fillScreen(ST77XX_BLACK);
  lcd.setTextWrap(true);  // enable text wrapping


  setDisplay(lcd);                    // utils_th helper → sets gfx
  setTextDefaults();

  return true;
}

void lcdClear(uint16_t colour)
{
  lcd.fillScreen(colour);
  lcd.setCursor(0, 0);
}
