#include "Arduino.h"
#include "MCPanel.h"
#include "esp32-hal.h"

extern "C" void espShow(uint8_t pin, uint8_t *pixels, uint32_t numBytes);

// uint8_t ReverseBits(uint8_t ch){
// ch = (ch & 0x55) << 1 | (ch >> 1) & 0x55;
// ch = (ch & 0x33) << 2 | (ch >> 2) & 0x33;
// ch = (ch & 0x0F) << 4 | (ch >> 4) & 0x0F;
// uint8_t chn = 0x00;
// chn |= ((ch & 0b10000000) >> 7)&0xff;
// chn |= ((ch & 0b01000000) >> 4)&0xff;
// chn |= ((ch & 0b00100000) << 1)&0xff;
// chn |= ((ch & 0b00010000) << 1)&0xff;
// chn |= ((ch & 0b00001000) << 1)&0xff;
// chn |= ((ch & 0b00000100) >> 1)&0xff;
// chn |= ((ch & 0b00000010) << 6)&0xff;
// chn |= ((ch & 0b00000001) << 3)&0xff;

// return chn;
// }

MCPanel::MCPanel() {
  
  STROBE_IO = TM1629_STROBE_PIN;
  DATA_IO = TM1629_DATA_PIN;
  CLOCK_IO = TM1629_CLOCK_PIN;

  LED_IO = TM1812_LED_PIN;
}

void MCPanel::begin(){

  expander.begin(0x20);

  expander.pinMode(STROBE_IO, OUTPUT);
  expander.pinMode(CLOCK_IO, OUTPUT);
  expander.pinMode(DATA_IO, OUTPUT);

  expander.pinMode(0, INPUT);
  expander.pinMode(1, INPUT);
  expander.pinMode(2, INPUT);
  expander.pinMode(3, INPUT);
  expander.pinMode(4, INPUT);
  expander.pinMode(5, INPUT);
  expander.pinMode(6, INPUT);
  expander.pinMode(7, INPUT);

  clearPos(0,15);
  sendCommand(ACTIVATE);

  oldButtons = readButtons();
  
  //TM1812-Part
  pinMode(LED_IO, OUTPUT);
  // rmt_send = rmtInit(LED_IO, true, RMT_MEM_64);
  // rmtSetTick(rmt_send, 100);
  for(int i=0;i<LED_NUM;i++)
    LEDCache[i] = 0x02;
  LEDCache[8] = 100;
  updateLED();

}

void MCPanel::sendCommand(uint8_t value)
{
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, value);
  expander.digitalWrite(STROBE_IO, HIGH);
}

void MCPanel::clearPos(uint8_t startPos, uint8_t stopPos, bool willUpdate){

  for (uint8_t i = startPos; i < (stopPos + 1); i++)
  {
    displayCache[i] = 0x00;
  }

  if(willUpdate) updateDisplay();
}

uint8_t MCPanel::shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;
	  //expander.digitalWrite(dataPin, HIGH);
    for(i = 0; i < 8; ++i) {
        expander.digitalWrite(clockPin, HIGH);
        uint16_t gpidRead = expander.read();
        encUpdate(gpidRead & 0xff);
        if(bitOrder == LSBFIRST)
            value |= ((gpidRead & (1 << dataPin)) ? HIGH : LOW) << i;
        else
            value |= ((gpidRead & (1 << dataPin)) ? HIGH : LOW) << (7 - i);
        expander.digitalWrite(clockPin, LOW);
    }
    return value;
}

uint32_t MCPanel::readButtons()
{
  uint32_t buttons = 0;
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, BUTTONS);
  expander.pinMode(DATA_IO, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = shiftIn(DATA_IO, CLOCK_IO, LSBFIRST);
    buttons |= (v << (i*8));
  }

  expander.pinMode(DATA_IO, OUTPUT);
  expander.digitalWrite(STROBE_IO, HIGH);
  return buttons;
}

void MCPanel::displayText(String text, uint8_t pos, bool willUpdate) {
  uint8_t length = text.length();
  if((length+pos) <= 16)
  {
    for (uint8_t i = 0; i < length; i++) {
        displayCache[i+pos] = ss[text[i]];
    }
    if(willUpdate) updateDisplay();
  }
    
}

void MCPanel::updateDisplay(){   ///OWN

  //sendCommand(WRITE_INC);
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t tempData = 0x00;  //SEG1-8
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[15-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);
    encUpdate(expander.read() & 0xff);
    tempData = 0x00;  //SEG9-10
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[7-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);
    encUpdate(expander.read() & 0xff);
  }
  expander.digitalWrite(STROBE_IO, HIGH);
  //sendCommand(ACTIVATE); // set auto increment mode
}

void MCPanel::displayNumber(uint8_t pos, int16_t value) {

  String textTemp = (String)value;
  switch(pos){
    case 3:
      clearPos(0,4,false);
      if(textTemp.length() <= 5){
        displayText(textTemp,5-textTemp.length(),false);
      }else{
        displayText("Err",1,false);
      }
      break;
    case 2:
      clearPos(5,7,false);
      if(textTemp.length() <= 3){
        displayText(textTemp,8-textTemp.length(),false);
      }else{
        displayText("Err",5,false);
      }
      break;
    case 1:
      clearPos(8,12,false);
      if(textTemp.length() <= 5){
        displayText(textTemp,13-textTemp.length(),false);
      }else{
        displayText("Err",9,false);
      }
      break;
    case 0:
      clearPos(13,15,false);
      if(textTemp.length() <= 3){
        displayText(textTemp,16-textTemp.length(),false);
      }else{
        displayText("Err",13,false);
      }

  }
  
  updateDisplay();

}

void MCPanel::changeCallbackFunc( void (*buttonUpFunc)(uint8_t), 
                                  void (*buttonDownFunc)(uint8_t),
                                  void (*encoderChange)(uint8_t, int)){

  uint32_t buttons = readButtons();
  uint32_t changes = (buttons ^ oldButtons);
  if (changes) {
    for (uint8_t i = 0; i < 32; i++) {
      uint32_t mask = (0x00000001 << i);
      if (mask & changes) {
        if (buttons & mask) {
          buttonDownFunc(i+1);
        }
        else {
          buttonUpFunc(i+1);
        }
      }
    }
    oldButtons = buttons;
  }
  for(uint8_t i=0;i<4;i++){
    if(encoderUpdateFlag & (1<<i)){
      if(enc_position[i] % 2 == 0){
        encoderChange(i,enc_position[i]/2);
        encoderUpdateFlag &= ~(1<<i);
      }
    }
  }
}

void MCPanel::updateLED(){

  espShow(TM1812_LED_PIN,LEDCache,LED_NUM);
  // rmt_data_t led_data[288];
  // int led, bit;
  // int i = 0;
  // for (led = 0; led < LED_NUM; led++) {
  //   for (bit = 0; bit < 8; bit++) {
  //     if ( LEDCache[led] & (1 << (8 - bit)) ) {
  //       led_data[i].level0 = 1;
  //       led_data[i].duration0 = 8;
  //       led_data[i].level1 = 0;
  //       led_data[i].duration1 = 4;
  //     } else {
  //       led_data[i].level0 = 1;
  //       led_data[i].duration0 = 4;
  //       led_data[i].level1 = 0;
  //       led_data[i].duration1 = 8;
  //     }
  //     i++;
  //   }

  // }
  // // Send the data
  // rmtWrite(rmt_send, led_data, 288);

}

void MCPanel::encUpdate(uint8_t res) {
  for (uint8_t which = 0; which < 4; which++) {
    uint8_t s = state[which] & 0b00001100;
    s |= (res >> (which * 2)) & 0b00000011;
    switch (s) {
      case 0: case 5: case 10: case 15:
        break;
      case 2: case 4: case 11: case 13:
        enc_position[which]++;
        encoderUpdateFlag |= (1 << which);
        break;
      case 1: case 7: case 8: case 14:
        enc_position[which]--;
        encoderUpdateFlag |= (1 << which);
        break;
      case 3: case 12:
        enc_position[which] += 2;
        encoderUpdateFlag |= (1 << which);
        break;
      default:
        enc_position[which] -= 2;
        encoderUpdateFlag |= (1 << which);
        break;
    }
    state[which] = (s << 2);
  }
}
