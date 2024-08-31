#ifndef keyboard_h
#define keyboard_h

#include "memory.h"
#include "utils.h"

#define KEYBOARD_MEM_ADDRESS 0x3800

void keyboardUtilities();
void loopKeyboard();
void printKeyboardUtilitiesMenu();

#endif