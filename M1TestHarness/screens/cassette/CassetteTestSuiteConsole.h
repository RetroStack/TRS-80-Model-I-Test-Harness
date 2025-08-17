#ifndef CASSETTE_TEST_SUITE_CONSOLE_H
#define CASSETTE_TEST_SUITE_CONSOLE_H

#include <ConsoleScreen.h>

class CassetteTestSuiteConsole : public ConsoleScreen {
 public:
  CassetteTestSuiteConsole();
  Screen* actionTaken(ActionTaken action, uint8_t offsetX, uint8_t offsetY) override;

 protected:
  void _executeOnce() override;

 private:
  void showConnectionInstructions();
  void runAllTests();
  void testRemoteControl();
  void testDataInputOutput();
  void testFlipFlopBehavior();
  void testPatternIntegrity();
};
#endif  // CASSETTE_TEST_SUITE_CONSOLE_H
