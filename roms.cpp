#include "roms.h"

void identifyROM(uint32_t crcValue, bool silent);

// Thanks to Ira @ trs-80.com
// https://www.trs-80.com/wordpress/roms/checksums-mod-1/
M1_ROMs romTable[] = {
    {"1.0\0", 0xAE5D, 0xDA84, 0x4002, 0xD8E9DFA7, "Y\0"},
    {"1.1\0", 0xAE60, 0xDA45, 0x40E0, 0x2DE3AFEC, "Y\0"},
    {"1.1\0", 0xAE60, 0xDA45, 0x3E3E, 0x4BE1227E, "Y\0"},
    {"1.2\0", 0xAE60, 0xDA45, 0x40BA, 0x0D8A132E, "Y\0"},
    {"1.3\0", 0xB078, 0xDA45, 0x4006, 0xA8E60D9A, "Y\0"},

    {"1.2\0", 0xAD8C, 0xDA45, 0x40BA, 0xB1ABA28D, "N\0"},
    {"1.2\0", 0xAD8C, 0xDA45, 0x40BA, 0xFDC1F12C, "N\0"},
    {"1.2\0", 0xAD8C, 0xDA45, 0x40BA, 0xD6FD9041, "N\0"},

    {"1.3\0", 0xAED7, 0xDA45, 0x4006, 0x39F02E2F, "N\0"}
};

int numberOfEntries = sizeof(romTable) / sizeof(romTable[0]);

// Reads 0x2FFF bytes from PINL and calculates the CRC32.
// TODO: should use a timer to time out after 2 seconds, switch to while loop
//uint32_t getROMCheckSum() {
void getROMInfo(bool silent)
{
  CRC32 crc;
  const uint32_t totalBytes = 0x2FFF;
  uint8_t data;

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    setTESTPin(LOW, silent);
  }

  pinMode(RAS_L, OUTPUT);
  digitalWrite(RAS_L, LOW);
  pinMode(RD_L, OUTPUT);
  digitalWrite(RD_L, LOW);

  // TODO: this is using older way of reading memory, since it is ROM, perhaps it doesn't matter
  // Not toggling the RAS and RD signals per each memory location unlike for SRAM and DRAM
  for (uint32_t i = 0; i <= totalBytes; ++i) {
      setAddressLinesToOutput(i); // Update address for each byte
      delayMicroseconds(5);

      // Simulate reading from PINL. Implement this part based on your actual data acquisition method.
      // For example, if PINL is directly mapped to a port from which you can read:
      data = PINF;
      
      if (0) {
        Serial.print(i, HEX);
        Serial.print(" ");
        Serial.println(data, HEX);
      }

      crc.update(data);
  }

  // Setup to write VRAM
  if (!inGlobalTestMode) {
    exitTestMode();
  } else {
    pinMode(RD_L, INPUT);
    pinMode(WR_L, INPUT);
    pinMode(RAS_L, INPUT);

    setAddressLinesToInput();
    setDataLinesToInput();
    asmWait(3);
  }
  identifyROM(crc.finalize(), silent);

  // return crc.finalize();
}

void identifyROM(uint32_t crcValue, bool silent)
{
  bool valid = false;

  SILENT_PRINT(silent, F("Known ROM versions: "));
  SILENT_PRINTLN(silent, numberOfEntries);

  SILENT_PRINT(silent, F("Checking if database has checksum/CRC32 value of: "));
  SILENT_PRINTLN(silent, crcValue, HEX);

  strcpy(romVersion, "Unknown\0");
  romChecksum = 0;  
  for (int i = 0; i < numberOfEntries; i++) {
    if (romTable[i].crc32 == crcValue) {
      SILENT_PRINT(silent, "ROM match found: ");
      SILENT_PRINT(silent, romTable[i].version);
      SILENT_PRINT(silent, ", valid: ");
      SILENT_PRINTLN(silent, romTable[i].valid);
      strcpy(romVersion, romTable[i].version);
      // romChecksum = crcValue;
      valid = true;
      break;
    }
  }

  // store copy of checksum
  romChecksum = crcValue;

  if (!valid) {
    SILENT_PRINTLN(silent, F("Unknown ROM - contact RetroStack"));
  }
}
