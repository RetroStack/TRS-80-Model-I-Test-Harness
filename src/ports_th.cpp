#include "ports_th.h"

#include "utils_th.h"  // println(), print(), MAX_INPUT_PARAMETERS, etc.

namespace PortsTH {

/* ======== Menu glue ==================================================== */
void showMenu() {
  println(F("s) Scan all ports"));
  println(F("c) Cassette port info"));
  println(F("v) Video port info"));
  println(F("r) RS/232 port info"));
  println(F("w) Write to port"));
  println(F("p) Read from port"));
  println(F("x) Back to main menu"));
  println(F("B) Bus pin statuses"));
  println(F("C) Clear screen"));
  // printTestSignalState();    // TODO
  println(F("X) Reset Test Harness"));
  println(F("?  help"));
}

void handleInput(const char* input) {
  if (!input || strlen(input) != 1) {
    println(F("Invalid input"));
    return;
  }

  char c = input[0];
  switch (c) {
    case 's':
      scanPorts();
      break;
    case 'c':
      cassettePortInfo();
      break;
    case 'v':
      videoPortInfo();
      break;
    case 'r':
      rs232PortInfo();
      break;
    case 'w':
      writePort();
      break;
    case 'p':
      readPort();
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

// Read a single port
void readPort() {
  println(F("Enter port address to read:"));
  print(F("> "));
  serialFlush();

  char* params[MAX_INPUT_PARAMETERS];
  uint8_t paramCount = readSerialInputParse(params, MAX_INPUT_PARAMETERS);
  serialFlush();

  if (paramCount == 0 || params[0] == nullptr) {
    println(F("No port address provided"));
    return;
  }

  uint8_t portAddr = (uint8_t)strToUint16(params[0]);
  if (portAddr > 0xFF) {
    println(F("Invalid port address (must be 0-255)"));
    return;
  }

  uint8_t value = Model1.readIO(portAddr);
  print(F("Port "), portAddr, F(" = "), value, F(" ("));
  printBinary(value, 8);
  println(F(")"));
}

// Write value to a port
void writePort() {
  println(F("Enter [port_address,value]:"));
  print(F("> "));
  serialFlush();

  char* params[MAX_INPUT_PARAMETERS];
  uint8_t paramCount = readSerialInputParse(params, MAX_INPUT_PARAMETERS);
  serialFlush();

  if (paramCount < 2 || params[0] == nullptr || params[1] == nullptr) {
    println(F("Need both port address and value"));
    return;
  }

  uint8_t portAddr = (uint8_t)strToUint16(params[0]);
  uint8_t value = (uint8_t)strToUint16(params[1]);

  if (portAddr > 0xFF || value > 0xFF) {
    println(F("Invalid port address or value (must be 0-255)"));
    return;
  }

  Model1.writeIO(portAddr, value);
  println(F("Wrote "), value, F(" to port "), portAddr);
}

/***********************************************************************
 *  scanPorts()  –  dump I/O map 0x00‑0xFF, 3 ports per line
 **********************************************************************/
void scanPorts() {
  printSeparator(F("PORT SCAN: 0x00‑0xFF"), '=', 64, 0);

  static const char hdr[] PROGMEM =
      "Port  Hex  ASCII  Binary     Port  Hex  ASCII  Binary     Port  Hex  ASCII  Binary";
  static const char rule[] PROGMEM =
      "----  ---  -----  --------   ----  ---  -----  --------   ----  ---  -----  --------";
  println(hdr);
  println(rule);

  for (uint16_t row = 0; row < 256; row += 3)  // walk 0x00‑0xFF
  {
    for (uint8_t col = 0; col < 3 && (row + col) < 256; ++col) {
      uint8_t port = uint8_t(row + col);
      uint8_t value = Model1.readIO(port);

      /* Port number “0xNN” */
      print(F("0x"));
      if (port < 0x10)
        print('0');
      print(port, Hex);

      /* Hex value “0xNN” */
      print(F(" 0x"));
      if (value < 0x10)
        print('0');
      print(value, Hex);

      /* ASCII column – always 5 chars */
      print(F("  "));
      if (value >= 32 && value <= 126) {
        print('\'');
        print(char(value));
        print('\'');
      } else {
        print(F(".  "));
      }

      /* Binary column – 8 bits MSB→LSB */
      print(' ');
      // for (int8_t bit = 7; bit >= 0; --bit)
      //   print((value >> bit) & 1, Bin);  // your base enum
      printBinary(value, 8);

      /* spacing between the three ports */
      if (col < 2 && (row + col + 1) < 256)
        print(F("   "));
    }

    println();

    if ((row & 0x1F) == 0)
      delay(10);  // throttle every 32 lines
  }

  printSeparator(F("SCAN COMPLETE"), '=', 64, 0);
}

void cassettePortInfo() {
  printSeparator(F("Cassette Port Info"), '=', 64, 0);
  detail::printCassetteStatus();
  printSeparator(F(""), '=', 64, 0);
}

void videoPortInfo() {
  printSeparator(F("Video Port Info"), '=', 64, 0);
  detail::printVideoStatus();
  printSeparator(F(""), '=', 64, 0);
}

void rs232PortInfo() {
  printSeparator(F("RS-232 Port Info"), '=', 64, 0);
  detail::printModemStatus();
  printSeparator(F(""), '=', 64, 0);
}

/* ... readPort(), writePort(), togglePin(), scanPorts() unchanged ... */

/* ======== Private helpers ============================================ */
namespace detail {

/* ---- E8/E9/EA/EB  (RS‑232 board) ------------------------------------ */
void printModemStatus() {
  const uint8_t e8 = Model1.readIO(0xE8);
  const uint8_t e9 = Model1.readIO(0xE9);
  const uint8_t ea = Model1.readIO(0xEA);
  const uint8_t eb = Model1.readIO(0xEB);

  println(F("[MODEM] 0xE8  Status/Signals  (raw): "), e8, Bin);
  print(F("        RXD="));
  println((e8 & 0x01) ? F("1") : F("0"));
  print(F("        RI ="));
  println((e8 & 0x10) ? F("1") : F("0"));
  print(F("        CD ="));
  println((e8 & 0x20) ? F("0") : F("1"));  // active low
  print(F("        DSR="));
  println((e8 & 0x40) ? F("1") : F("0"));
  print(F("        CTS="));
  println((e8 & 0x80) ? F("1") : F("0"));

  println(F("\n[MODEM] 0xE9  Sense Switches  (raw): "), e9, Bin);
  print(F("        WordLen = "));
  switch ((e9 >> 5) & 0x03) {
    case 0:
      println(F("5"));
      break;
    case 1:
      println(F("6"));
      break;
    case 2:
      println(F("7"));
      break;
    case 3:
      println(F("8"));
      break;
  }
  print(F("        StopBits = "));
  println((e9 & 0x10) ? F("2") : F("1"));
  print(F("        Parity   = "));
  if (e9 & 0x10)
    println(F("Disabled"));
  else
    println((e9 & 0x80) ? F("Even") : F("Odd"));
  print(F("        Baud     = "));
  printBaud(e9 & 0x07);

  println(F("\n[MODEM] 0xEA  UART Status    (raw): "), ea, Bin);
  print(F("        Unused bits (0-2): "));
  print((ea & 0x01) ? F("x") : F("x"));
  print((ea & 0x02) ? F("x") : F("x"));
  println((ea & 0x04) ? F("x") : F("x"));
  print(F("        ParityErr  = "));
  println((ea & 0x08) ? F("Yes") : F("No"));
  print(F("        FrameErr   = "));
  println((ea & 0x10) ? F("Yes") : F("No"));
  print(F("        OverrunErr = "));
  println((ea & 0x20) ? F("Yes") : F("No"));
  print(F("        Sent(TBMT) = "));
  println((ea & 0x40) ? F("Yes") : F("No"));
  print(F("        DataReady   = "));
  println((ea & 0x80) ? F("Yes") : F("No"));

  println(F("\n[MODEM] 0xEB  RX Data       : 0x"), eb, Hex, F(" ('"),
          (eb >= 32 && eb < 127 ? (char)eb : '.'), F("')"));
}

/* translate 3‑bit DIP combination (Model‑1) into baud rate */
void printBaud(uint8_t swBits) {
  static const uint16_t baudLut[8] PROGMEM = {110, 150, 300, 600, 1200, 2400, 4800, 9600};
  if (swBits < 8) {
    println((uint16_t)pgm_read_word(&baudLut[swBits]));
  } else {
    println(F("Unknown"));
  }
}

/* ---- FF  (Cassette) ------------------------------------ */
// TODO: switch to use M1 Arduino library once cassette status support is added
void printCassetteStatus() {
  const uint8_t portFF = Model1.readIO(0xFF);
  print(F("[CASSETTE/VIDEO] 0xFF (raw): "));
  printBinary(portFF, 8);

  // Bits 0-1: Cassette voltage level
  uint8_t voltageBits = portFF & 0x03;  // Extract bits 0-1
  print(F("        Cassette Voltage (bits 0-1): "));
  switch (voltageBits) {
    case 0b00:
      println(F("00 - ≈ 0.85V"));
      break;
    case 0b01:
      println(F("01 - ≈ 0.46V"));
      break;
    case 0b10:
      println(F("10 - 0V (ground)"));
      break;
    case 0b11:
      println(F("11 - 0V (ground)"));
      break;
  }

  // Bit 2: Cassette Motor on/off
  print(F("        Cassette Motor (bit 2): "));
  println((portFF & 0x04) ? F("ON") : F("OFF"));

  // Bit 3: 64/32 character mode for video
  print(F("  Video Character Mode (bit 3): "));
  println((portFF & 0x08) ? F("64 char") : F("32 char"));

  // Bits 4-7: Unused (but show in case of HW modifications)
  print(F("        Unused bits (4-7): "));
  print((portFF & 0x10) ? F("1") : F("0"));
  print((portFF & 0x20) ? F("1") : F("0"));
  print((portFF & 0x40) ? F("1") : F("0"));
  println((portFF & 0x80) ? F("1") : F("0"));
  println(F("        *Bit 7 wired for input"));
}

void printVideoStatus() {
  const uint8_t portFF = Model1.readIO(0xFF);
  println(F("[VIDEO] 0xFF (raw): "), portFF, Bin);

  // Bit 3: 64/32 character mode
  print(F("\tCharacter Mode (bit 3): "));
  println((portFF & 0x08) ? F("64 char") : F("32 char"));

  print(F("\tCassette uses bits 0-2:"));

  // Bits 4-7: Unused (but show in case of HW modifications)
  print(F("\tUnused bits (4-7): "));
  print((portFF & 0x10) ? F("1") : F("0"));
  print((portFF & 0x20) ? F("1") : F("0"));
  print((portFF & 0x40) ? F("1") : F("0"));
  println((portFF & 0x80) ? F("1") : F("0"));
  println(F("        *Bit 7 wired for input"));
}

}  // namespace detail

}  // namespace PortsTH
