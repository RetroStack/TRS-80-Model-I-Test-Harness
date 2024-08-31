#include <stdint.h>
#ifndef SHIELD_PINS_H
#define SHIELD_PINS_H

#include "Arduino.h"
#include "utils.h"

#define RAS_L     2
#define CAS_L     3
#define IN_L      18
#define RD_L      19  // Defined as PD2 on Port D
#define OUT_L     38
#define WR_L      40
#define CR2       42
#define CR1       43
#define INTAK_L   44
#define RST_L     45
#define WAIT_L    46
#define TEST_L    47
#define INT_L     48
#define MUX_L     49 

// Not marked on shield
#define BUSAK_L  41

void displayCtrlPinStatus();
void initControlPins();
void setAddressLinesToInput();
void setAddressLinesToOutput(uint16_t memAddress);
void setAllPinsPortsToInput();
void setDataLinesToInput();
void setDataLinesToOutput();
void setTESTPin(int state, bool silent=true);
void turnOffReadWriteRASLines();

#endif