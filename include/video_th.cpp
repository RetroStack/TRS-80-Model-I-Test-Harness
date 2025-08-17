#include "video_th.h"

#include "globals_th.h"

namespace VideoTH {

void showMenu() {
  println(TO_LCD, F("d) Display test"));
  println(TO_LCD, F("c) Character test"));
  println(TO_LCD, F("g) Graphics test"));
  println(TO_LCD, F("m) Character mode, 64 or 32"));
  println(TO_LCD, F("t) TESTS ->"));
  println(TO_LCD, F("x) Back to main menu"));
  println(TO_LCD, F("B) Bus pin statuses"));
  println(TO_LCD, F("C) Clear screen"));
  // printTestSignalState();    // TODO
  println(TO_LCD, F("X) Reset Test Harness"));
  println(TO_LCD, F("?  help"));
}

// Handle input for the video menu
void handleInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(TO_LCD, F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case 't':
      Menu::push(Menu::NodeID::VIDEO_TESTS);
      break;
    case 'd':
      println(TO_LCD, F("[VIDEO] Display test - TODO"));
      break;
    case 'c':
      println(TO_LCD, F("[VIDEO] Character test - TODO"));
      break;
    case 'g':
      println(TO_LCD, F("[VIDEO] Graphics test - TODO"));
      break;
    case 'm':
      toggleCharacterMode();
      break;  // Toggle between 64 and 32 character mode
    case 'x':
      Menu::pop();
      break;
    case '?': /* Menu system auto-shows */
      break;
    default:
      println(TO_LCD, F("Invalid input"));
      break;
  }
}

// TODO: sample menu
void showTests() {
  println(TO_LCD, F("1) Pixel test"));
  println(TO_LCD, F("2) Line test"));
  println(TO_LCD, F("3) Pattern test"));
  println(TO_LCD, F("4) Color test"));
  println(TO_LCD, F("x) Back"));
  println(TO_LCD, F("?  Help"));
  print(TO_LCD, F("> "));
}

// Handle input for video tests
void handleTestsInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(TO_LCD, F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case '1':
      println(TO_LCD, F("[VIDEO] Pixel test - TODO"));
      break;
    case '2':
      println(TO_LCD, F("[VIDEO] Line test - TODO"));
      break;
    case '3':
      println(TO_LCD, F("[VIDEO] Pattern test - TODO"));
      break;
    case '4':
      println(TO_LCD, F("[VIDEO] Color test - TODO"));
      break;
    case 'x':
      Menu::pop();
      break;
    case '?': /* Menu system auto-shows */
      break;
    default:
      println(TO_LCD, F("Invalid input"));
      break;
  }
}

// Toggle between 64 and 32 character mode
void toggleCharacterMode() {
  video.cls();
  if (cassette.is64CharacterMode()) {
    cassette.set32CharacterMode();
    println(TO_LCD, F("Switched to 32-character mode"));
    video.println(F("S w i t c h e d  t o  3 2 - c h a r a c t e r  m o d e"));

  } else {
    cassette.set64CharacterMode();
    video.println(F("Switched to 64-character mode"));
  }
  // print(TO_LCD, F("Current mode: "));
  // println(cassette.is64CharacterMode() ? F("64") : F("32"));
}
}  // namespace VideoTH
