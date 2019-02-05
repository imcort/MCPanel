#include <MCPanel.h>
#include "Wire.h"

MCPanel mcp;

void setup() {
  Serial.begin(115200);
  pinMode(33, INPUT_PULLUP);
  mcp.begin();

  Serial.println(Wire.getClock(), DEC);

  mcp.displayNumber(0,0);
  mcp.displayNumber(1,0);
  mcp.displayNumber(2,0);
  mcp.displayNumber(3,0);
  mcp.clearPos(9,12);
  mcp.updateLED();
}

void func1(bool i, uint8_t j) {

  switch(i){
    case 0:
      Serial.print("Key Pressed:");
      break;
    case 1:
      Serial.print("Key Released:");
  }
  Serial.println(j + 1);
  mcp.displayNumber(0,j);
  
}

void func2(uint8_t i, int j) {
  Serial.print("Encoder ");
  Serial.print(i);
  Serial.print(" Changed:");
  Serial.println(j);
  mcp.displayNumber(i,j);
}

void loop() {

  mcp.changeCallbackFunc(func1, func2);
 
}