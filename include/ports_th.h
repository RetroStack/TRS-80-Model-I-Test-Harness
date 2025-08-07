#ifndef PORTS_TH_H
#define PORTS_TH_H

#include <Arduino.h>

#include "globals_th.h"
#include "menu_th.h"
#include "utils_th.h"

namespace PortsTH {
/* ---------- public API used by the menu engine ---------- */
void showMenu();                      // print submenu (called on entry)
void handleInput(const char *input);  // dispatch input (memory‑safe version)

/* ---------- port utility functions ---------- */
void readPort();          // p – read single port
void writePort();         // w – write single port
void scanPorts();         // s – dump all ports (raw)
void cassettePortInfo();  // c – cassette port detailed info
void videoPortInfo();     // v – video port detailed info
void rs232PortInfo();     // r – RS-232 port detailed info
void togglePin();         // t – toggle specific Arduino pin

/* ---------- helpers (internal) --------------------------- */
namespace detail {
void printModemStatus();         // E8‑EB block
void printCassetteStatus();      // FF
void printVideoStatus();         // FF
void printBaud(uint8_t swBits);  // helper for dips on 0xE9 (Model‑1 only)
}  // namespace detail

}  // namespace PortsTH
#endif  // PORTS_TH_H
