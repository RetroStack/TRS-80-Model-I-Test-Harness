# Project TODOs

General
- [] input routines, convert lc to uc or such to help slim code
- [] A window to play on M1 with DRAM refresh running
- [] Space after comma in input causing problem
- [] Look at pollSerial() and use it for parsing/parameter in utils file
- [] seperator standard:  ===, ---, ...

System Configuration
- [*] Display fontset
- [o] Detect memory size and use in system such as RAM tests
- [*] Lowercase detection
- [*] Video in 64 or 32 mode

Keyboard - finished
- [*] key test
- [*] stuck key test

ROM - finished
- [*] show start addresses
- [*] byte(s) read, write
- [*] dump ROM to serial port
- [*] ROM length
- [*] version ID
- [*] checksums

RAM
- [*] readMemory
- [*] writeMemory
- [] *readMemory - returns buffer, need to delete[] afterwards
- [*] writeMemory
- [] writeMemory with offset
- [*] copyMemory
- [*] fillMemory - byte
- [o] fillMemory - pattern - seems to be a bug TODO
- [*] memory tests
- [] upload data from serial port

Cassette
- [] update, need to modify library to get state
- [*] toggle motor
- [] play sound
- [] read program
- [] write program
- [*] sync cassette state

Video
- [] Character set display - TBD
- [*] toggle 32/64
- [] get current mode
- [] sync state to TH (see cassette docs)
- []

Port
- [*] write
- [*] read
- [*] rs-232 ports status
- [] rs-232 read/write?
- [*] cassette port status
- [*] scan all ports
- [*] video port

Shield
- [] BUSAK*
- [] Buttons
- [] LED - cut traces
- [o] LCD - I have text output to LCD display, not using your library (yet)


Arduino Library
- [] video.print() video.println() - support 32 character mode, automatically add space


## Bugs
- [] Model1.getState() memory leak?
- [] Model1.activateWaitSignal(); // Activate WAIT signal
- [] Model1.activateInterruptRequestSignal(); // Activate interrupt request signal
- [] seems to be bug in scrolling. main menu followed by rom menu, bottom part of col 0 are blank

## Ideas
- [] Add real help
- [] Pin activation
