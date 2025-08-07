#ifndef CASSETTE_TH_H
#define CASSETTE_TH_H

#include <Arduino.h>

#include "globals_th.h"
#include "menu_th.h"
#include "utils_th.h"

namespace CassTH {
/* ---------- public API used by the menu engine ---------- */
void showMenu();                      // print submenu (called on entry)
void showSongsMenu();                 // print songs submenu
void handleInput(const char* input);  // dispatch input (memory-safe version)
void syncCassetteState();             // sync cassette state with Test Harness

/* ---------- cassette functions ---------- */
void toggleMotor();   // m - toggle motor on/off
void playSound();     // p - play sound
void loadProgram();   // r - load a program [TBD]
void writeProgram();  // w - write a program [TBD]

/* ---------- song functions ---------- */
void playImperial();       // Play Imperial March
void playTetris();         // Play Tetris theme
void playMario();          // Play Super Mario theme
void playUnderworld();     // Play Underworld theme
void playSimpsons();       // Play Simpsons theme
void playGameOfThrones();  // Play Game of Thrones theme
void playDoom();           // Play Doom theme
void playPacman();         // Play Pacman theme

/* ---------- helper functions ---------- */
void handleMainMenuInput(const char* input);
void handleSongsInput(const char* input);

/* ---------- external song data ---------- */
extern const int imperialMelody[];
extern const float imperialDurations[];
extern const size_t imperialMelodyLength;

extern const int tetrisMelodyLead[];
extern const float tetrisDurationsLead[];
extern const size_t tetrisMelodyLength;

extern const int marioMelody[];
extern const float marioDurations[];
extern const size_t marioMelodyLength;

extern const int underworldMelody[];
extern const float underworldDurations[];
extern const size_t underworldMelodyLength;

extern const int simpsonsMelody[];
extern const float simpsonsDurations[];
extern const size_t simpsonsMelodyLength;

extern const int gameOfThronesMelody[];
extern const float gameOfThronesDurations[];
extern const size_t gameOfThronesMelodyLength;

extern const int doomMelody[];
extern const float doomDurations[];
extern const size_t doomMelodyLength;

extern const int pacmanMelody[];
extern const float pacmanDurations[];
extern const size_t pacmanMelodyLength;

/* ---------- cassette play function ---------- */
void cassettePlaysong(const int melody[], const float durations[], size_t length);

}  // namespace CassTH

#endif  // CASSETTE_TH_H
