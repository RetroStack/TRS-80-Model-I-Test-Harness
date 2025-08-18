#include "./CassetteSongPlayerMenu.h"

#include <Arduino.h>

#include "../../globals.h"
#include "./CassetteMenu.h"
#include "./CassetteSongData.h"

CassetteSongPlayerMenu::CassetteSongPlayerMenu() : MenuScreen() {
  setTitleF(F("Song Player"));

  const __FlashStringHelper *menuItems[] = {
      F("Imperial March"), F("Tetris Theme"),   F("Super Mario"),     F("Pac-Man"),
      F("Underworld"),     F("Simpsons Theme"), F("Game of Thrones"), F("Doom Theme")};
  setMenuItemsF(menuItems, 8);

  initializeSongs();

}

void CassetteSongPlayerMenu::initializeSongs() {
  _songs[0] = {"Imperial March", imperialMelody, imperialDurations, imperialMelodyLength, 120};
  _songs[1] = {"Tetris Theme", tetrisMelodyLead, tetrisDurationsLead, tetrisMelodyLength, 140};
  _songs[2] = {"Super Mario", marioMelody, marioDurations, marioMelodyLength, 180};
  _songs[3] = {"Pac-Man", pacmanMelody, pacmanDurations, pacmanMelodyLength, 120};
  _songs[4] = {"Underworld", underworldMelody, underworldDurations, underworldMelodyLength, 150};
  _songs[5] = {"Simpsons Theme", simpsonsMelody, simpsonsDurations, simpsonsMelodyLength, 160};
  _songs[6] = {"Game of Thrones", gameOfThronesMelody, gameOfThronesDurations,
               gameOfThronesMelodyLength, 100};
  _songs[7] = {"Doom Theme", doomMelody, doomDurations, doomMelodyLength, 120};
}

void CassetteSongPlayerMenu::playSong(int songIndex) {
  if (songIndex >= 0 && songIndex < MAX_SONGS && _songs[songIndex].name != nullptr) {
    Globals.logger.infoF(F("Playing song: %s"), _songs[songIndex].name);

    // Activate test signal for cassette operations
    Model1.deactivateMemoryRefresh();
    Model1.activateTestSignal();
    Globals.logger.infoF(F("Test signal activated for song playback"));

    // Play the song using PROGMEM-aware function (this is blocking - will return when complete)
    Globals.cassette.playSongPGM(_songs[songIndex].melody, _songs[songIndex].durations,
                                 _songs[songIndex].length, _songs[songIndex].tempo);

    // Deactivate test signal after playback
    Model1.deactivateTestSignal();
    Model1.activateMemoryRefresh();
    Globals.logger.infoF(F("Test signal deactivated after song playback"));

    Globals.logger.infoF(F("%s playback complete"), _songs[songIndex].name);
  }
}

Screen *CassetteSongPlayerMenu::_getSelectedMenuItemScreen(int index) {
  switch (index) {
    case 0:  // Imperial March
      playSong(0);
      return nullptr;  // Stay on this screen

    case 1:  // Tetris Theme
      playSong(1);
      return nullptr;

    case 2:  // Super Mario
      playSong(2);
      return nullptr;

    case 3:  // Pac-Man
      playSong(3);
      return nullptr;

    case 4:  // Underworld
      playSong(4);
      return nullptr;

    case 5:  // Simpsons Theme
      playSong(5);
      return nullptr;

    case 6:  // Game of Thrones
      playSong(6);
      return nullptr;

    case 7:  // Doom Theme
      playSong(7);
      return nullptr;

    case -1:  // Back to Main
      return new CassetteMenu();

    default:
      return nullptr;
  }
}
