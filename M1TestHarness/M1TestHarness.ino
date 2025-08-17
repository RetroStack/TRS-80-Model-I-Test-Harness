
#include <Arduino.h>

#include "./globals.h"
#include "./screens/WelcomeConsole.h"
// #include "./screens/MainMenu.h"
#include "retro_stack_240x240.h"

// First, tell the system which display you have
// For ST7789 240x320 displays (most common, landscape becomes 320x240)
#include <Display_ST7789_320x240.h>
Display_ST7789_320x240 displayProvider;

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

void drawLogo(Adafruit_GFX &gfx, uint16_t width, uint16_t height) {
  gfx.fillScreen(0x000F);  // background similar to your logo’s bg

  // Centered position for 240x240 on a 320x240 screen
  const int16_t x = (width - RETRO_STACK_WIDTH) / 2;
  const int16_t y = (height - RETRO_STACK_HEIGHT) / 2;

  // Draw the PROGMEM RGB565 bitmap
  // Many Adafruit_GFX drivers support reading RGB565 from PROGMEM with drawRGBBitmap.
  gfx.drawRGBBitmap(x, y, (const uint16_t *)retro_stack_240x240, RETRO_STACK_WIDTH,
                    RETRO_STACK_HEIGHT);
}

void setup() {
  Serial.begin(115200);

  Model1.begin(-1);  // 2);  // Setup Timer 2
  M1Shield.begin(displayProvider);

  // Enable joystick input for navigation
  // Uncomment to allow joystick control of menus and screens
  //   M1Shield.activateJoystick();

  Globals.logger.infoF(F("=== TRS-80 Model 1 Testharness ==="));

  // Show logo
  drawLogo(displayProvider.getGFX(), displayProvider.width(), displayProvider.height());
  delay(2000);

  // Start with the welcome console
  M1Shield.setScreen(new WelcomeConsole());
}

ISR(TIMER2_COMPA_vect) {
  Model1.nextUpdate();
}

void loop() {
  M1Shield.loop();
}
