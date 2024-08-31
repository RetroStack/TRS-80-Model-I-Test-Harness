#include "utils.h"

char binaryString[9];
char stringBuffer[MAX_BUFFER_SIZE];       // used for string manipulation and display
char inputBuffer[MAX_BUFFER_SIZE];
char *parameters[MAX_INPUT_PARAMETERS];
int  parameterCount = 0;
bool inGlobalTestMode = false; // State variable for test mode status
char romVersion[10] = "?\0";
uint32_t romChecksum = 0;

const char* sramChips[8] = {"Z63", "Z62", "Z61", "Z45", "Z46", "Z47", "PIG", "Z48"};
const char* dramChips[8] = {"Z13", "Z14", "Z220", "Z15", "Z19", "Z18", "Z16", "Z17"};

// This assumes ATMega 2560, 1 cycle = 62.5ns
// The main loop is 3 cycles, excluding the mov
// Min = 250ns, Max = 12266812.5ns (12.2668125 ms)
// Example: wait = 10, then delay will be 10us 
void asmWait(uint16_t wait) {
  __asm__ volatile (
    " mov r16,%0\n"
    "1: nop\n"
    " dec r16\n"
    " brne 1b\n"              // 1 cycle if branching, 2 if not
    :
    : "r"(wait)              // input operands if any, here
    : "r16"   // clobbered regs here
  );
}

void asmWait(uint16_t outerLoopCount, uint16_t innerLoopCount) {
    asm volatile (
        "outer_loop_start: \n\t"          // Outer loop start label
        "movw r24, %A0 \n\t"              // Copy outer loop count to r24:r25
        "inner_loop_start: \n\t"          // Inner loop start label
        "movw r26, %A1 \n\t"              // Copy inner loop count to r26:r27
        "inner_loop: \n\t"                // Inner loop label
        "sbiw r26, 1 \n\t"                // Subtract one from the inner loop count
        "brne inner_loop \n\t"            // Branch to Inner loop label if zero flag is clear
        "sbiw r24, 1 \n\t"                // Subtract one from the outer loop count
        "brne inner_loop_start \n\t"      // Branch to Inner loop start label if zero flag is clear
        : "+w" (outerLoopCount), "+w" (innerLoopCount)  // Outputs: modified in place
        : "r" (outerLoopCount), "r" (innerLoopCount)    // Inputs
        : "r24", "r25", "r26", "r27"                    // Clobbers
    );
}


// Base case: When no more arguments are left, print a newline.
void printLine()
{
    Serial.println();
}

uint8_t inputPrompt(const char* str)
{
  Serial.println(str);

  // flush out
  while (Serial.available() > 0) {
    Serial.read();
  }
  
  while(!Serial.available()) {
    // do nothing
  }

  uint8_t firstChar = Serial.read();
  firstChar = toLowerCase(firstChar);
  
  serialFlush();

  return firstChar;
}

// TODO: phase this out
void readSerialInput(char *buffer, int bufferSize) {
    int index = 0;
    while (index < bufferSize - 1) {
        while (Serial.available() == 0) {}  // Wait for input
        char ch = Serial.read();
        if (ch == '\n' || ch == '\r') break;
        buffer[index++] = ch;
    }
    buffer[index] = '\0';
}

// Function to read the input from the serial port and split it into an array
void readSerialInput2() {
  int index = 0;
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      inputBuffer[index] = '\0';  // Null-terminate the string
      break;
    } else {
      inputBuffer[index++] = inChar;
      if (index >= MAX_BUFFER_SIZE - 1) {
        inputBuffer[index] = '\0';  // Ensure the buffer is null-terminated
        break;
      }
    }
  }

  // Split the inputBuffer into parameters
  char *token = strtok(inputBuffer, ",");
  parameterCount = 0;
  while (token != NULL && parameterCount < MAX_INPUT_PARAMETERS) {
    parameters[parameterCount++] = token;
    token = strtok(NULL, ",");
  }
}

// Function to convert character to lowercase
char toLowerCase(char c) {
  if (c >= 'A' && c <= 'Z') {
      return c + ('a' - 'A');
  }
  return c;
}

// Display a number in binary format
void sprintfBinary(uint8_t value) {
  for (int i = 7; i >= 0; i--) {
    binaryString[7 - i] = (value & (1 << i)) ? '1' : '0';
  }
  binaryString[8] = '\0'; // Null terminator
}


void serialFlush(){
  // Serial.println("Flushing serial.");
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

// TODO: find better name for this
void enterTestMode(bool silent) {
  setTESTPin(LOW);

  // TODO: support BUSAK* wired to shield 
  if (!isBUSAKWired) {
    SILENT_PRINT(silent, F("Waiting "));
    SILENT_PRINT(silent, CPU_BUS_WAIT * 187.5);
    SILENT_PRINT(silent, F(" ns for 40 pin bus to be free."));
    // TODO: need to ensure this wait works all/most of the time
    asmWait(CPU_BUS_WAIT);
  } else {
    SILENT_PRINT(silent, F("Waiting for BUSAK* pin to go low - make sure it is connected to pin XX on Test Harness."));
    while (true) {
      int pinState = digitalRead(BUSAK_L);
      if (pinState == LOW) break;
      SILENT_PRINT(silent, F("BUSAK*:\t"));
      SILENT_PRINTLN(pinState ? F("HIGH") : F("ACTIVE_LOW"));
    }
  }
}


// TODO: find better name for this
void exitTestMode(bool silent) {
  SILENT_PRINTLN(silent, F("Exiting test mode. Setting to inputs: RD*, RAS*, WR*, A0-A15, D0-D9."));

  turnOffReadWriteRASLines();
  
  setAddressLinesToInput();
  setDataLinesToInput();
  setTESTPin(HIGH);

  // TODO: support if BUSAK* wired up
  if (0) {
    int pinState = digitalRead(BUSAK_L);
    printLine(F("BUSAK*: "), pinState ? F("HIGH") : F("ACTIVE_LOW"));
  }
}

void convertHexStringToByteArray(char* hexString, unsigned char* byteArray) {
    char buffer[3];
    int length = strlen(hexString);
    buffer[2] = '\0';
    for (int i = 0; i < length; i++) {
        buffer[0] = hexString[2 * i];
        buffer[1] = hexString[2 * i + 1];
        byteArray[i] = (unsigned char) strtoul(buffer, NULL, 16);
    }
}

