#ifndef KEYBOARD_TH_H
#define KEYBOARD_TH_H

#include "globals_th.h"
#include "utils_th.h"

namespace KeyboardTH
{

    void showMenu();                    // prints the submenu
    void handleInput(const char* input); // user input (C-style string)

    void clearScreen();
    void keyTest();
    void stuckKeyTest(uint16_t timeoutSeconds = 15);
};

#endif // KEYBOARD_TH_H