#ifndef DRAM_TEST_SUITE_CONSOLE_H
#define DRAM_TEST_SUITE_CONSOLE_H

#include "../RAMTestSuiteConsole.h"

/**
 * DRAMTestSuite - Specialized RAM testing for Dynamic RAM
 *
 * This class inherits from RAMTestSuite and focuses specifically on testing the
 * TRS-80 Model 1's dynamic RAM expansion. The DRAM size is configurable and
 * determined by the current DRAM menu selection (4KB, 8KB, 16KB, 32KB, or 48KB).
 * Memory is mapped starting from 0x4000, implemented using 8 DRAM chips.
 *
 * IC References:
 * - Z17, Z16, Z18, Z19, Z15, Z20, Z14, Z13 (8 DRAM chips)
 *
 * Features:
 * - Comprehensive DRAM-specific testing using selected size
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
