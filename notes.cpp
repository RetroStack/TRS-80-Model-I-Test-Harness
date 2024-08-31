/*

* It seems with Arduino, setting a PIN to INPUT doesn't always make the signal
  it was when it was an OUTPUT go 'neutral' as the M1 signals are mostly
  active LOW. It seems best to force such signal to HIGH before setting to
  INPUTS. This most critical when reading as last operation could have been
  a write. This may also have to do with the M1 test mode leaving the signals
  floating. 

- When repeatedly calling routine to get ROM checksum/version, value at 3C00H changes randomly.
  Same issue for VRAM Dump, but that was 'fixed' by force WR_L to be high before the read.
  This fix is the approach to be used for other routines.

* TODO
  - need to get DRAM refresh working
  - DRAM test utilities
  - detect RAM installed
  - cassette utilties (toggle relay)
  - toggle between 32/64 characters
  - on memory range inputs, make sure hex addresses max out at FFFF
  - make '0' as the default way to clear the TRS-80 screen

*/
