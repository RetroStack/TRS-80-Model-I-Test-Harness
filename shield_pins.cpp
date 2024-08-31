#include "shield_pins.h"

// Display status of control pins
void displayCtrlPinStatus() {
  const int pins[] = {BUSAK_L, CAS_L, CR1, CR2, IN_L, INTAK_L, INT_L, MUX_L, OUT_L, RAS_L, RD_L, RST_L, WAIT_L, WR_L};
  const char* pinNames[] = {"BUSAK_L", "CAS_L", "CR1", "CR2", "IN_L", "INTAK_L", "INT_L", "MUX_L", "OUT_L", "RAS_L", "RD_L", "RST_L", "WAIT_L", "WR_L"};
  const int pinCount = sizeof(pins) / sizeof(pins[0]);
    
  for (int i = 0; i < pinCount; ++i) {
    pinMode(pins[i], INPUT); // Set the pin mode to input
    int state = digitalRead(pins[i]);
    Serial.print(pinNames[i]);
    Serial.print(": ");
    Serial.println(state == HIGH ? "HIGH" : "LOW");
  }
}

// Init Test Harness control pins
void initControlPins() {
  pinMode(BUSAK_L, INPUT);
  pinMode(CAS_L, INPUT);
  pinMode(CR1, INPUT);
  pinMode(CR2, INPUT);
  pinMode(IN_L, INPUT);
  pinMode(INTAK_L, INPUT);
  pinMode(INT_L, INPUT);
  pinMode(MUX_L, INPUT);
  pinMode(OUT_L, INPUT);
  pinMode(RAS_L, INPUT);
  pinMode(RD_L, INPUT);
  pinMode(RST_L, INPUT);
  pinMode(WAIT_L, INPUT);
  pinMode(WR_L, INPUT);
}

void setAllPinsPortsToInput() {
  printLine("Settting all pins and ports to INPUT.");
  initControlPins();
  setAddressLinesToInput();
  setDataLinesToInput();
}

void setAddressLinesToInput() {
  DDRA = 0x00;    // lower 8
  DDRC = 0x00;    // upper 8
}

void setAddressLinesToOutput(uint16_t memAddress) {
  DDRA = 0xFF;    // lower 8
  DDRC = 0xFF;    // upper 8

  PORTA = (uint8_t)(memAddress & 0xFF);
  PORTC = (uint8_t)((memAddress >> 8) & 0xFF);
}

void setDataLinesToInput() {
  DDRF = 0x00;    // 8 bits
}

void setDataLinesToOutput() {
  DDRF = 0xFF;    // 8 bits
}

void setTESTPin(int state, bool silent) {
  // TEST pin is active low
  if (state == 0) {   
    SILENT_PRINTLN(silent, "Setting TEST* to LOW (active).");
    pinMode(TEST_L, OUTPUT);
    digitalWrite(TEST_L, LOW);
  } else {
    SILENT_PRINTLN(silent, "Setting TEST* to HIGH (inactive).");
    pinMode(TEST_L, OUTPUT);
    digitalWrite(TEST_L, HIGH);
    pinMode(TEST_L, INPUT);
  }
  
  // TODO: need to check on scope
  asmWait(20);
}

void turnOffReadWriteRASLines() {
  pinMode(WR_L, OUTPUT);        // turn off write asap
  digitalWrite(WR_L, HIGH);
  pinMode(WR_L, INPUT);

  pinMode(RD_L, OUTPUT);
  digitalWrite(RD_L, HIGH);
  pinMode(RD_L, INPUT);

  pinMode(RAS_L, OUTPUT);
  digitalWrite(RAS_L, HIGH);
  pinMode(RAS_L, INPUT);

  asmWait(3);
}

