#include "Arduino.h"
#include "MCPanel.h"


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

MCPanel::MCPanel(uint8_t strobe, uint8_t clock, uint8_t data) {
  
  STROBE_IO = strobe;
  DATA_IO = data;
  CLOCK_IO = clock;

}

void MCPanel::begin(){

  expander.begin(0x20);

  expander.pinMode(STROBE_IO, OUTPUT);
  expander.pinMode(CLOCK_IO, OUTPUT);
  expander.pinMode(DATA_IO, OUTPUT);

  sendCommand(ACTIVATE);
  reset();

  oldButtons = readButtons();

}

void MCPanel::sendCommand(uint8_t value)
{
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, value);
  expander.digitalWrite(STROBE_IO, HIGH);
}

void MCPanel::reset() {
  sendCommand(WRITE_INC); // set auto increment mode
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 16; i++)
  {
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0x00);
  }
  for (uint8_t i = 0; i < 16; i++)
  {
    displayCache[i] = 0x00;
  }
  expander.digitalWrite(STROBE_IO, HIGH);
  sendCommand(WRITE_INC); // set auto increment mode
  sendCommand(ACTIVATE);
}

uint32_t MCPanel::readButtons()
{
  uint32_t buttons = 0;
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, BUTTONS);
  expander.pinMode(DATA_IO, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = expander.shiftIn(DATA_IO, CLOCK_IO, LSBFIRST);
    buttons |= (v << (i*8));
  }

  expander.pinMode(DATA_IO, OUTPUT);
  expander.digitalWrite(STROBE_IO, HIGH);
  return buttons;
}

void MCPanel::displayText(String text) {
  uint8_t length = text.length();
  if(length <= 16)
  {
    for (uint8_t i = 0; i < length; i++) {
        displayCache[i] = ss[text[i]];
        //displayASCII(i, text[i]);
    }
    updateDisplay();
  }
    
}

void MCPanel::displaySS(uint8_t position, uint8_t value) { // call 7-segment
  displayCache[position] = value;//ReverseBits(value);
  updateDisplay();
}

void MCPanel::updateDisplay(){   ///OWN

  sendCommand(WRITE_INC);
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t tempData = 0x00;  //SEG1-8
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[15-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);

    tempData = 0x00;  //SEG9-10
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[7-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);

  }
  expander.digitalWrite(STROBE_IO, HIGH);
  sendCommand(ACTIVATE); // set auto increment mode
}

void MCPanel::displayASCII(uint8_t position, uint8_t ascii) {
  displaySS(position, ss[ascii]);
}

void MCPanel::displayHex(uint8_t position, uint8_t hex) {
  displaySS(position, hexss[hex]);
}

void MCPanel::displayNumber(uint16_t alt, uint16_t spd, int16_t vs, uint16_t hdg) {

  if(alt < 100000){

    displayCache[0] = hexss[alt/10000];
    displayCache[1] = hexss[alt/1000%10];
    displayCache[2] = hexss[alt/100%10];
    displayCache[3] = hexss[alt/10%10];
    displayCache[4] = hexss[alt/1%10];

  }
  if(spd < 1000){

    displayCache[5] = hexss[spd/100];
    displayCache[6] = hexss[spd/10%10];
    displayCache[7] = hexss[spd/1%10];

  }
  if((vs > 0) && (vs < 10000)){

    displayCache[8] = 0x00;
    displayCache[9] = hexss[vs/1000];
    displayCache[10] = hexss[vs/100%10];
    displayCache[11] = hexss[vs/10%10];
    displayCache[12] = hexss[vs/1%10];

  }

  if((vs < 0) && (vs > -10000)){

    displayCache[8] = ss['-'];
    displayCache[9] = hexss[-vs/1000];
    displayCache[10] = hexss[-vs/100%10];
    displayCache[11] = hexss[-vs/10%10];
    displayCache[12] = hexss[-vs/1%10];

  }
  if(hdg < 1000){

    displayCache[13] = hexss[hdg/100];
    displayCache[14] = hexss[hdg/10%10];
    displayCache[15] = hexss[hdg/1%10];

  }
  updateDisplay();

}

void MCPanel::buttonsCallbackFunc(void (*buttonUpFunc)(uint8_t), void (*buttonDownFunc)(uint8_t)){

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

}
