#ifndef DRAM_TEST_SUITE_CONSOLE_H
#define DRAM_TEST_SUITE_CONSOLE_H

#include "../RAMTestSuiteConsole.h"

/**
 * DRAMTestSuite - Specialized RAM testing for Dynamic RAM (16KB at 0x4000-0x7FFF)
 *
 * This class inherits from RAMTestSuite and focuses specifically on testing the
 * TRS-80 Model 1's dynamic RAM expansion. The DRAM typically consists of 16KB
 * of memory mapped from 0x4000 to 0x7FFF, implemented using 8 DRAM chips.
 *
 * IC References:
 * - Z17, Z16, Z18, Z19, Z15, Z20, Z14, Z13 (8 DRAM chips)
 *
 * Features:
 * - Comprehensive DRAM-specific testing
 * - Progress tracking during tests
 * - Per-bit error analysis mapped to specific ICs
 * - Automatic TEST signal control
 */
class DRAMTestSuiteConsole : public RAMTestSuiteConsole {
 public:
  DRAMTestSuiteConsole();
  Screen *actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;
};

#endif  // DRAM_TEST_SUITE_CONSOLE_H
