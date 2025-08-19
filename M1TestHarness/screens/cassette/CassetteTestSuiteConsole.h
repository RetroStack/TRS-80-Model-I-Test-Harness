#ifndef CASSETTE_TEST_SUITE_CONSOLE_H
#define CASSETTE_TEST_SUITE_CONSOLE_H

#include <ConsoleScreen.h>

class CassetteTestSuiteConsole : public ConsoleScreen {
 public:
  CassetteTestSuiteConsole();
  Screen* actionTaken(ActionTaken action, int8_t offsetX, int8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void showConnectionInstructions();
  void runAllTests();
  void testRemoteControl();
  void testDataOutput();
  void testDataInput();
  void testCassettePinConfiguration();
  void testAnalogCassetteInterface();
  void testPatternIntegrity();
  void testTimingAndFrequency();
  void testVoltageRange();
  void testSignalIntegrity();
  void testCrosstalk();
  void testEndurance();
};
#endif  // CASSETTE_TEST_SUITE_CONSOLE_H
