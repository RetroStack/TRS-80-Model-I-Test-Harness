#include "cassette_th.h"

#include <avr/pgmspace.h>  // For PROGMEM and pgm_read_* functions

#include "utils_th.h"  // Ensure extern parameters is visible

namespace CassTH {

static bool motorOn = false;  // Track motor state

void showMenu() {
  print(F("m) Toggle motor "));
  if (motorOn) {
    println(F("[ON]"));
  } else {
    println(F("[OFF]"));
  }
  println(TO_LCD, F("p) Play songs →"));
  println(TO_LCD, F("r) Load a program from tape [TBD]"));
  println(TO_LCD, F("s) Sync cassette state -> Test Harness"));
  println(TO_LCD, F("w) Write a program from tape [TBD]"));
  println(TO_LCD, F("x) Back to main menu"));
  println(TO_LCD, F("B) Bus pin statuses"));
  println(TO_LCD, F("C) Clear screen"));
  // printTestSignalState(); TODO
  println(TO_LCD, F("X) Reset Test Harness"));
  println(TO_LCD, F("?  help"));
}

void handleInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(F("Invalid input"));
    return;
  }

  // Handle based on current menu context
  switch (Menu::current()) {
    case Menu::NodeID::CAS_SONGS:
      handleSongsInput(input);
      break;
    case Menu::NodeID::CAS:
      handleMainMenuInput(input);
      break;
    default:
      println(F("Unknown menu state"));
      break;
  }
}

void handleMainMenuInput(const char* input) {
  char c = input[0];
  switch (c) {
    case 'm':
      toggleMotor();
      break;
    case 'p':
      Menu::push(Menu::NodeID::CAS_SONGS);
      break;
    case 'r':
      loadProgram();
      break;
    case 's':
      syncCassetteState();
      break;
    case 'w':
      writeProgram();
      break;
    case 'x':
      Menu::pop();
      break;
    case '?': /* Menu system auto-shows */
      break;
    default:
      println(F("Invalid input"));
      break;
  }
}

void handleSongsInput(const char* input) {
  if (isCommand(input, "x")) {
    Menu::pop();
    return;
  }

  if (isCommand(input, "?")) {
    // Menu system will auto-show current menu
    return;
  }

  char c = input[0];
  switch (c) {
    case '1':
      playImperial();
      break;
    case '2':
      playTetris();
      break;
    case '3':
      playMario();
      break;
    case '4':
      playUnderworld();
      break;
    case '5':
      playSimpsons();
      break;
    case '6':
      playGameOfThrones();
      break;
    case '7':
      playDoom();
      break;
    case '8':
      playPacman();
      break;
    default:
      println(F("Invalid input, ? for help."));
      break;
  }
}

// Toggle cassette motor and state
void toggleMotor() {
  motorOn = !motorOn;
  print(TO_LCD, F("Cassette motor turned "));
  if (motorOn) {
    println(TO_LCD, F("ON"));
    cassette.activateRemote();
  } else {
    println(TO_LCD, F("OFF"));
    cassette.deactivateRemote();
  }
}

void syncCassetteState() {
  // This function is a placeholder for syncing cassette state with Test Harness
  // Currently, it just prints the current motor state
  print(TO_LCD, F("Syncing cassette state: Motor is "));
  cassette.update();
}

void playSound() {
  println(TO_LCD, F("[CASSETTE] Playing sound - TODO"));
  // TODO: Implement sound playback functionality
}

void loadProgram() {
  println(TO_LCD, F("[CASSETTE] Load program - TBD"));
  // TODO: Implement program loading from cassette
}

void writeProgram() {
  println(TO_LCD, F("[CASSETTE] Write program - TBD"));
  // TODO: Implement program writing to cassette
}

// Songs submenu
void showSongsMenu() {
  println(TO_LCD, F("1) Imperial March"));
  println(TO_LCD, F("2) Tetris Theme"));
  println(TO_LCD, F("3) Super Mario Theme"));
  println(TO_LCD, F("4) Underworld Theme"));
  println(TO_LCD, F("5) Simpsons Theme"));
  println(TO_LCD, F("6) Game of Thrones Theme"));
  println(TO_LCD, F("7) Doom Theme"));
  println(TO_LCD, F("8) Pacman Theme"));
  println(TO_LCD, F("x) Back"));
  println(TO_LCD, F("?  Help"));
  print(TO_LCD, F("> "));
}

// Song implementations
void playImperial() {
  println(TO_LCD, F("[CASSETTE] Playing Imperial March..."));
  cassettePlaysong(imperialMelody, imperialDurations, imperialMelodyLength);
}

void playTetris() {
  println(TO_LCD, F("[CASSETTE] Playing Tetris Theme..."));
  cassettePlaysong(tetrisMelodyLead, tetrisDurationsLead, tetrisMelodyLength);
}

void playMario() {
  println(TO_LCD, F("[CASSETTE] Playing Super Mario Theme..."));
  cassettePlaysong(marioMelody, marioDurations, marioMelodyLength);
}

void playUnderworld() {
  println(TO_LCD, F("[CASSETTE] Playing Underworld Theme..."));
  cassettePlaysong(underworldMelody, underworldDurations, underworldMelodyLength);
}

void playSimpsons() {
  println(TO_LCD, F("[CASSETTE] Playing Simpsons Theme..."));
  cassettePlaysong(simpsonsMelody, simpsonsDurations, simpsonsMelodyLength);
}

void playGameOfThrones() {
  println(TO_LCD, F("[CASSETTE] Playing Game of Thrones Theme..."));
  cassettePlaysong(gameOfThronesMelody, gameOfThronesDurations, gameOfThronesMelodyLength);
}

void playDoom() {
  println(TO_LCD, F("[CASSETTE] Playing Doom Theme..."));
  cassettePlaysong(doomMelody, doomDurations, doomMelodyLength);
}

void playPacman() {
  println(TO_LCD, F("[CASSETTE] Playing Pacman Theme..."));
  cassettePlaysong(pacmanMelody, pacmanDurations, pacmanMelodyLength);
}

const int imperialMelody[] PROGMEM = {
    NOTE_A4, NOTE_A4,  NOTE_A4, NOTE_A4, NOTE_A4,  NOTE_A4,  NOTE_F4,
    REST,    NOTE_A4,  NOTE_A4, NOTE_A4, NOTE_A4,  NOTE_A4,  NOTE_A4,
    NOTE_F4, REST,     NOTE_A4, NOTE_A4, NOTE_A4,  NOTE_F4,  NOTE_C5,

    NOTE_A4, NOTE_F4,  NOTE_C5, NOTE_A4, NOTE_E5,  NOTE_E5,  NOTE_E5,
    NOTE_F5, NOTE_C5,  NOTE_A4, NOTE_F4, NOTE_C5,  NOTE_A4,

    NOTE_A5, NOTE_A4,  NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5,  NOTE_DS5,
    NOTE_D5, NOTE_DS5, REST,    NOTE_A4, NOTE_DS5, NOTE_D5,  NOTE_CS5,

    NOTE_C5, NOTE_B4,  NOTE_C5, REST,    NOTE_F4,  NOTE_GS4, NOTE_F4,
    NOTE_A4, NOTE_C5,  NOTE_A4, NOTE_C5, NOTE_E5,

    NOTE_A5, NOTE_A4,  NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5,  NOTE_DS5,
    NOTE_D5, NOTE_DS5, REST,    NOTE_A4, NOTE_DS5, NOTE_D5,  NOTE_CS5,

    NOTE_C5, NOTE_B4,  NOTE_C5, REST,    NOTE_F4,  NOTE_GS4, NOTE_F4,
    NOTE_A4, NOTE_A4,  NOTE_F4, NOTE_C5, NOTE_A4};

const float imperialDurations[] PROGMEM = {
    0.375,  0.375,  0.0625, 0.0625, 0.0625, 0.0625, 0.125,  0.125,   0.375,  0.375,  0.0625, 0.0625,
    0.0625, 0.0625, 0.125,  0.125,  0.25,   0.25,   0.25,   0.1875,  0.0625,

    0.25,   0.1875, 0.0625, 0.5,    0.25,   0.25,   0.25,   0.1875,  0.0625, 0.25,   0.1875, 0.0625,
    0.5,

    0.25,   0.1875, 0.0625, 0.25,   0.1875, 0.0625, 0.0625, 0.0625,  0.125,  0.125,  0.125,  0.25,
    0.1875, 0.0625,

    0.0625, 0.0625, 0.0625, 0.125,  0.125,  0.25,   0.1875, 0.09375, 0.25,   0.1875, 0.0625, 0.5,

    0.25,   0.1875, 0.0625, 0.25,   0.1875, 0.0625, 0.0625, 0.0625,  0.125,  0.125,  0.125,  0.25,
    0.1875, 0.0625,

    0.0625, 0.0625, 0.0625, 0.125,  0.125,  0.25,   0.1875, 0.09375, 0.25,   0.1875, 0.0625, 0.5};

const size_t imperialMelodyLength = sizeof(imperialMelody) / sizeof(int);

const int tetrisMelodyLead[] PROGMEM = {
    NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4,  NOTE_A4, NOTE_C5, NOTE_E5,
    NOTE_D5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,  NOTE_C5, NOTE_A4, NOTE_A4,
    REST,

    NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_C5,  NOTE_E5, NOTE_D5, NOTE_C5,
    NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_A4,  NOTE_A4, REST,

    NOTE_E4, NOTE_C4, NOTE_D4, NOTE_B3, NOTE_C4, NOTE_A3, NOTE_GS3, NOTE_B3, NOTE_E4, NOTE_C4,
    NOTE_D4, NOTE_B3, NOTE_C4, NOTE_E4, NOTE_A4, NOTE_A4, NOTE_GS4, REST};

const float tetrisDurationsLead[] PROGMEM = {
    // part 1
    0.25,  0.125, 0.125, 0.25,  0.125, 0.125, 0.25,  0.125, 0.125, 0.25,  0.125,
    0.125, 0.25,  0.125, 0.125, 0.25,  0.25,  0.25,  0.25,  0.25,  0.25,

    0.375, 0.125, 0.25,  0.125, 0.125, 0.375, 0.125, 0.25,  0.125, 0.125, 0.25,
    0.125, 0.125, 0.25,  0.25,  0.25,  0.25,  0.25,  0.25,

    0.5,   0.5,   0.5,   0.5,   0.5,   0.5,   0.5,   0.5,   0.5,   0.5,   0.5,
    0.5,   0.25,  0.25,  0.25,  0.25,  0.75,  0.25};

const size_t tetrisMelodyLength = sizeof(tetrisMelodyLead) / sizeof(int);

const int marioMelody[] PROGMEM = {
    NOTE_E5, NOTE_E5, REST,    NOTE_E5, REST,    NOTE_C5,  NOTE_E5, REST,
    NOTE_G5, REST,    REST,    REST,    NOTE_G4, REST,     REST,    REST,

    NOTE_C5, REST,    REST,    NOTE_G4, REST,    REST,     NOTE_E4, REST,
    REST,    NOTE_A4, REST,    NOTE_B4, REST,    NOTE_AS4, NOTE_A4, REST,

    NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, REST,    NOTE_F5,  NOTE_G5, REST,
    NOTE_E5, REST,    NOTE_C5, NOTE_D5, NOTE_B4, REST,     REST,

    NOTE_C5, REST,    REST,    NOTE_G4, REST,    REST,     NOTE_E4, REST,
    REST,    NOTE_A4, REST,    NOTE_B4, REST,    NOTE_AS4, NOTE_A4, REST,

    NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, REST,    NOTE_F5,  NOTE_G5, REST,
    NOTE_E5, REST,    NOTE_C5, NOTE_D5, NOTE_B4, REST,     REST};

const float marioDurations[] PROGMEM = {
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,

    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,

    0.1111111, 0.1111111, 0.1111111, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,

    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,

    0.1111111, 0.1111111, 0.1111111, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333};

const size_t marioMelodyLength = sizeof(marioMelody) / sizeof(int);

const int underworldMelody[] PROGMEM = {
    NOTE_C4,  NOTE_C5,  NOTE_A3,  NOTE_A4,  NOTE_AS3, NOTE_AS4, REST,     REST,
    NOTE_C4,  NOTE_C5,  NOTE_A3,  NOTE_A4,  NOTE_AS3, NOTE_AS4, REST,     REST,
    NOTE_F3,  NOTE_F4,  NOTE_D3,  NOTE_D4,  NOTE_DS3, NOTE_DS4, REST,     REST,
    NOTE_F3,  NOTE_F4,  NOTE_D3,  NOTE_D4,  NOTE_DS3, NOTE_DS4, REST,     REST,
    NOTE_DS4, NOTE_CS4, NOTE_D4,  NOTE_CS4, NOTE_DS4, NOTE_DS4, NOTE_GS3, NOTE_G3,
    NOTE_CS4, NOTE_C4,  NOTE_FS4, NOTE_F4,  NOTE_E3,  NOTE_AS4, NOTE_A4,  NOTE_GS4,
    NOTE_DS4, NOTE_B3,  NOTE_AS3, NOTE_A3,  NOTE_GS3, REST,     REST,     REST};

const float underworldDurations[] PROGMEM = {
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.1666667, 0.3333333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.1666667, 0.3333333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.1666667, 0.3333333,
    0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.0833333, 0.1666667, 0.1666667,
    0.0555556, 0.0555556, 0.0555556, 0.1666667, 0.1666667, 0.1666667, 0.1666667, 0.1666667,
    0.1666667, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.0555556, 0.1,
    0.1,       0.1,       0.1,       0.1,       0.1,       0.3333333, 0.3333333, 0.3333333};

const size_t underworldMelodyLength = sizeof(underworldMelody) / sizeof(int);

const int simpsonsMelody[] PROGMEM = {NOTE_C4, NOTE_E4, NOTE_FS4, NOTE_A4,  NOTE_G4,  NOTE_E4,
                                      NOTE_C4, NOTE_G3, NOTE_FS3, NOTE_FS3, NOTE_FS3, NOTE_G3};

const float simpsonsDurations[] PROGMEM = {
    0.5,    // C4 (half)
    0.25,   // E4 (quarter)
    0.25,   // FS4 (quarter)
    0.125,  // A4 (eighth)
    0.5,    // G4 (half)
    0.25,   // E4 (quarter)
    0.25,   // C4 (quarter)
    0.25,   // G3 (quarter)
    0.125,  // FS3 (eighth)
    0.125,  // FS3 (eighth)
    0.125,  // FS3 (eighth)
    0.25    // G3 (quarter)
};

const size_t simpsonsMelodyLength = sizeof(simpsonsMelody) / sizeof(int);

const int gameOfThronesMelody[] PROGMEM = {
    NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4, NOTE_G4,
    NOTE_C4,  NOTE_DS4, NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4,  NOTE_G4, NOTE_C4,
    NOTE_E4,  NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_E4,  NOTE_F4,  NOTE_G4,  NOTE_C4, NOTE_E4,
    NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_E4,  NOTE_F4,  NOTE_G4,  NOTE_C4,

    NOTE_DS4, NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4,  NOTE_D4,  NOTE_F4, NOTE_AS3,
    NOTE_DS4, NOTE_D4,  NOTE_F4,  NOTE_AS3, NOTE_DS4, NOTE_D4,  NOTE_C4,

    NOTE_G4,  NOTE_C4,

    NOTE_DS4, NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4,  NOTE_D4,  NOTE_F4, NOTE_AS3,
    NOTE_DS4, NOTE_D4,  NOTE_F4,  NOTE_AS3, NOTE_DS4, NOTE_D4,  NOTE_C4,  NOTE_G4, NOTE_C4,
    NOTE_DS4, NOTE_F4,  NOTE_G4,  NOTE_C4,  NOTE_DS4, NOTE_F4,

    NOTE_D4,  NOTE_F4,  NOTE_AS3, NOTE_D4,  NOTE_DS4, NOTE_D4,  NOTE_AS3, NOTE_C4, NOTE_C5,
    NOTE_AS4, NOTE_C4,  NOTE_G4,  NOTE_DS4, NOTE_DS4, NOTE_F4,  NOTE_G4,

    NOTE_C5,  NOTE_AS4, NOTE_C4,  NOTE_G4,  NOTE_DS4, NOTE_DS4, NOTE_D4,  NOTE_C5, NOTE_G4,
    NOTE_GS4, NOTE_AS4, NOTE_C5,  NOTE_G4,  NOTE_GS4, NOTE_AS4, NOTE_C5,  NOTE_G4, NOTE_GS4,
    NOTE_AS4, NOTE_C5,  NOTE_G4,  NOTE_GS4, NOTE_AS4,

    REST,     NOTE_GS5, NOTE_AS5, NOTE_C6,  NOTE_G5,  NOTE_GS5, NOTE_AS5, NOTE_C6, NOTE_G5,
    NOTE_GS5, NOTE_AS5, NOTE_C6,  NOTE_G5,  NOTE_GS5, NOTE_AS5};

const float gameOfThronesDurations[] PROGMEM = {
    0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625,
    0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625,
    0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625, 0.25,   0.25,

    0.0625, 0.0625, 0.25,   0.25,   0.0625, 0.0625, 1.0,    0.25,   0.25,   0.0625, 0.0625, 0.25,
    0.25,   0.0625, 0.0625, 1.0,

    0.25,   0.25,

    0.0625, 0.0625, 0.25,   0.25,   0.0625, 0.0625, 1.0,    0.25,   0.25,   0.0625, 0.0625, 0.25,
    0.25,   0.0625, 0.0625, 1.0,    0.25,   0.25,   0.0625, 0.0625, 0.25,   0.25,   0.0625, 0.0625,

    0.5,    0.25,   0.25,   0.125,  0.125,  0.125,  0.125,  1.0,    0.5,    0.5,    0.5,    0.5,
    0.5,    0.25,   0.25,   1.0,

    0.5,    0.5,    0.5,    0.5,    0.5,    0.25,   0.25,   0.125,  0.125,  0.0625, 0.0625, 0.125,
    0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625,

    0.25,   0.0625, 0.0625, 0.125,  0.125,  0.0625, 0.0625, 0.125,  0.0625, 0.0625, 0.0625, 0.125,
    0.125,  0.0625, 0.0625};

const size_t gameOfThronesMelodyLength = sizeof(gameOfThronesMelody) / sizeof(int);

const int doomMelody[] PROGMEM = {
    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_FS3, NOTE_D3,  NOTE_B2,
    NOTE_A3,  NOTE_FS3, NOTE_B2,  NOTE_D3,  NOTE_FS3, NOTE_A3,  NOTE_FS3, NOTE_D3,  NOTE_B2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_B3,  NOTE_G3,  NOTE_E3,
    NOTE_G3,  NOTE_B3,  NOTE_E4,  NOTE_G3,  NOTE_B3,  NOTE_E4,  NOTE_B3,  NOTE_G4,  NOTE_B4,

    NOTE_A2,  NOTE_A2,  NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_F3,
    NOTE_A2,  NOTE_A2,  NOTE_DS3, NOTE_A2,  NOTE_A2,  NOTE_E3,  NOTE_F3,  NOTE_A2,  NOTE_A2,
    NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_F3,  NOTE_A2,  NOTE_A2,
    NOTE_DS3,

    NOTE_A2,  NOTE_A2,  NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_F3,
    NOTE_A2,  NOTE_A2,  NOTE_DS3, NOTE_A2,  NOTE_A2,  NOTE_E3,  NOTE_F3,  NOTE_A2,  NOTE_A2,
    NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_A3,  NOTE_F3,  NOTE_D3,
    NOTE_A3,  NOTE_F3,  NOTE_D3,  NOTE_C4,  NOTE_A3,  NOTE_F3,  NOTE_A3,  NOTE_F3,  NOTE_D3,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_CS3, NOTE_CS3, NOTE_CS4, NOTE_CS3, NOTE_CS3, NOTE_B3,  NOTE_CS3, NOTE_CS3, NOTE_A3,
    NOTE_CS3, NOTE_CS3, NOTE_G3,  NOTE_CS3, NOTE_CS3, NOTE_GS3, NOTE_A3,  NOTE_B2,  NOTE_B2,
    NOTE_B3,  NOTE_B2,  NOTE_B2,  NOTE_A3,  NOTE_B2,  NOTE_B2,  NOTE_G3,  NOTE_B2,  NOTE_B2,
    NOTE_F3,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_B3,  NOTE_G3,  NOTE_E3,
    NOTE_G3,  NOTE_B3,  NOTE_E4,  NOTE_G3,  NOTE_B3,  NOTE_E4,  NOTE_B3,  NOTE_G4,  NOTE_B4,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_FS3, NOTE_DS3, NOTE_B2,
    NOTE_FS3, NOTE_DS3, NOTE_B2,  NOTE_G3,  NOTE_D3,  NOTE_B2,  NOTE_DS4, NOTE_DS3, NOTE_B2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_E4,  NOTE_B3,  NOTE_G3,
    NOTE_G4,  NOTE_E4,  NOTE_G3,  NOTE_B3,  NOTE_D4,  NOTE_E4,  NOTE_G4,  NOTE_E4,  NOTE_G3,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_A2,  NOTE_A2,  NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_F3,
    NOTE_A2,  NOTE_A2,  NOTE_DS3, NOTE_A2,  NOTE_A2,  NOTE_E3,  NOTE_F3,  NOTE_A2,  NOTE_A2,
    NOTE_A3,  NOTE_A2,  NOTE_A2,  NOTE_G3,  NOTE_A2,  NOTE_A2,  NOTE_A3,  NOTE_F3,  NOTE_D3,
    NOTE_A3,  NOTE_F3,  NOTE_D3,  NOTE_C4,  NOTE_A3,  NOTE_F3,  NOTE_A3,  NOTE_F3,  NOTE_D3,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_AS2,

    NOTE_E2,  NOTE_E2,  NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_C3,
    NOTE_E2,  NOTE_E2,  NOTE_AS2, NOTE_E2,  NOTE_E2,  NOTE_B2,  NOTE_C3,  NOTE_E2,  NOTE_E2,
    NOTE_E3,  NOTE_E2,  NOTE_E2,  NOTE_D3,  NOTE_E2,  NOTE_E2,  NOTE_B3,  NOTE_G3,  NOTE_E3,
    NOTE_B2,  NOTE_E3,  NOTE_G3,  NOTE_C4,  NOTE_B3,  NOTE_G3,  NOTE_B3,  NOTE_G3,  NOTE_E3};

const float doomDurations[] PROGMEM = {
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625, 0.0625,

    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,  0.125,
    0.125,  0.125,  0.125,  0.5};

const size_t doomMelodyLength = sizeof(doomMelody) / sizeof(int);

const int pacmanMelody[] PROGMEM = {
    NOTE_B4, NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_C5,  NOTE_C6,
    NOTE_G6, NOTE_E6, NOTE_C6,  NOTE_G6,  NOTE_E6,

    NOTE_B4, NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_DS5, NOTE_E5,
    NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_G5,  NOTE_G5, NOTE_GS5, NOTE_A5,  NOTE_B5};

const float pacmanDurations[] PROGMEM = {
    0.0625,  0.0625,  0.0625,  0.0625,  0.03125, 0.0625,  0.125,  0.0625,  0.0625,
    0.0625,  0.0625,  0.03125, 0.0625,  0.125,

    0.0625,  0.0625,  0.0625,  0.0625,  0.03125, 0.0625,  0.125,  0.03125, 0.03125,
    0.03125, 0.03125, 0.03125, 0.03125, 0.03125, 0.03125, 0.0625, 0.125};

const size_t pacmanMelodyLength = sizeof(pacmanMelody) / sizeof(int);

// Implementation of cassette song playback function
void cassettePlaysong(const int melody[], const float durations[], size_t length) {
  println(F("[CASSETTE] Starting song playback..."));

  // Copy data from PROGMEM to RAM for the TRS-80 library
  int* tempMelody = new int[length];
  float* tempDurations = new float[length];

  for (size_t i = 0; i < length; i++) {
    tempMelody[i] = pgm_read_word(&melody[i]);
    tempDurations[i] = pgm_read_float(&durations[i]);
  }

  // Use the TRS-80 Cassette library's playSong method
  // Default to 120 BPM for a reasonable tempo
  cassette.playSong(tempMelody, tempDurations, length, 120);

  // Clean up temporary arrays
  delete[] tempMelody;
  delete[] tempDurations;

  println(F("[CASSETTE] Song playback complete."));
}

}  // namespace CassTH
