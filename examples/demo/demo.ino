#include <MCPanel.h>
#include "Wire.h"

MCPanel mcp;

void setup() {
  Serial.begin(115200);
  pinMode(33, INPUT_PULLUP);
  mcp.begin();

  Serial.println(Wire.getClock(), DEC);

  mcp.displayNumber(240, 350, -1000, 100);
  mcp.clearPos(9,12);
  mcp.updateLED();
}

void func1(uint8_t i) {
  Serial.print("Key Pressed:");
  Serial.println(i + 1);
  mcp.displayNumber(i + 1, 350, -1000, 10);
}

void func2(uint8_t i) {
  Serial.print("Key Released:");
  Serial.println(i + 1);
  mcp.displayNumber(i + 1, 350, -1000, 10);
}

void func3(uint8_t i, int j) {
  Serial.print("Encoder ");
  Serial.print(i);
  Serial.print(" Changed:");
  Serial.println(j);
  mcp.displayNumber(j, 350, -1000, 10);
}

void loop() {

  mcp.changeCallbackFunc(func2, func1, func3);
 
}