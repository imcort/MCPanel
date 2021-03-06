/*
  TM1638-lite.h - Library for addressing TM1638 I/O chips/card from Arduino.
  Danny Ayers (@danja), 2018.
  Apache 2 license.
*/
#ifndef MCPanel_h
#define MCPanel_h

#include "Arduino.h"
#include "PCF8575.h" 
#include "esp32-hal.h"

///Interface Defines:
#define TM1629_STROBE_PIN   8
#define TM1629_CLOCK_PIN    9
#define TM1629_DATA_PIN     10

#define TM1812_LED_PIN      32

//TM1629 Defines:
#define BRIGHTNESS 1
#define ACTIVATE (0x88 | BRIGHTNESS)
#define BUTTONS 0x42
#define WRITE_LOC 0x44
#define WRITE_INC 0x40


#define DIGIT_NUM 16

//TM1812 Defines:
#define LED_NUM 36

#define KEY_1 4
  #define KEY_1_W 9
#define KEY_2 8
  #define KEY_2_W 10
#define KEY_3 12
  #define KEY_3_W 11
#define KEY_4 16
  #define KEY_4_W 6

#define KEY_5 20

#define KEY_6 24
#define KEY_7 28

#define KEY_8 32

#define KEY_9 3
#define KEY_10 7

#define KEY_11 11

#define KEY_12 19
#define KEY_13 15

#define KEY_14 23

#define KEY_15 27
#define KEY_16 31

#define KEY_17 2
#define KEY_18 6

#define KEY_19 10
  #define KEY__W 29
#define KEY_20 14
  #define KEY__W 28
#define KEY_21 30
  #define KEY__W 27
#define KEY_22 26
  #define KEY__W 26
#define KEY_23 22
  #define KEY__W 25
#define KEY_24 18
  #define KEY__W 24

class MCPanel
{
  public:
    MCPanel();

    void begin();
    void sendCommand(uint8_t value);
    uint32_t readButtons();
    void clearPos(uint8_t startPos, uint8_t stopPos, bool willUpdate = true);
    void displayText(String text, int8_t pos = 0, bool willUpdate = true);
    void updateDisplay();
    void displayNumber(uint8_t pos, int16_t value);
    void changeCallbackFunc(void (*buttonChange)(bool, uint8_t), 
                            void (*encoderChange)(uint8_t, int));
    void updateLED();
    void encUpdate(uint8_t res);
    void setLED(uint8_t pos, uint8_t value, bool willUpdate = true);

  private:

    uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);
    uint8_t displayCache[DIGIT_NUM];
    PCF8575 expander;
    uint8_t STROBE_IO;
    uint8_t DATA_IO;
    uint8_t CLOCK_IO;
    uint8_t LED_IO;

    //TM1812
    uint8_t LEDCache[LED_NUM];
    //rmt_obj_t* rmt_send;
    uint32_t oldButtons;
    //Encoder
    uint8_t state[4];
    uint8_t encoderUpdateFlag;
    int enc_position[4];
    
};

/*
const uint8_t keyValue[] = {
  0,//0
  

}
*/
// map of ASCII values to 7-segment
const uint8_t ss[] = {
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // padding for non-char ASCII
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // 0x00, /* (space) */
  // 0x86, /* ! */
  // 0x22, /* " */
  // 0x7E, /* # */
  // 0x6D, /* $ */
  // 0xD2, /* % */
  // 0x46, /* & */
  // 0x20, /* ' */
  // 0x29, /* ( */
  // 0x0B, /* ) */
  // 0x21, /* * */
  // 0x70, /* + */
  // 0x10, /* , */
  // 0x40, /* - */
  // 0x80, /* . */
  // 0x52, /* / */
  // 0x3F, /* 0 */
  // 0x06, /* 1 */
  // 0x5B, /* 2 */
  // 0x4F, /* 3 */
  // 0x66, /* 4 */
  // 0x6D, /* 5 */
  // 0x7D, /* 6 */
  // 0x07, /* 7 */
  // 0x7F, /* 8 */
  // 0x6F, /* 9 */
  // 0x09, /* : */
  // 0x0D, /* ; */
  // 0x61, /* < */
  // 0x48, /* = */
  // 0x43, /* > */
  // 0xD3, /* ? */
  // 0x5F, /* @ */
  // 0x77, /* A */
  // 0x7C, /* B */
  // 0x39, /* C */
  // 0x5E, /* D */
  // 0x79, /* E */
  // 0x71, /* F */
  // 0x3D, /* G */
  // 0x76, /* H */
  // 0x30, /* I */
  // 0x1E, /* J */
  // 0x75, /* K */
  // 0x38, /* L */
  // 0x15, /* M */
  // 0x37, /* N */
  // 0x3F, /* O */
  // 0x73, /* P */
  // 0x6B, /* Q */
  // 0x33, /* R */
  // 0x6D, /* S */
  // 0x78, /* T */
  // 0x3E, /* U */
  // 0x3E, /* V */
  // 0x2A, /* W */
  // 0x76, /* X */
  // 0x6E, /* Y */
  // 0x5B, /* Z */
  // 0x39, /* [ */
  // 0x64, /* \ */
  // 0x0F, /* ] */
  // 0x23, /* ^ */
  // 0x08, /* _ */
  // 0x02, /* ` */
  // 0x5F, /* a */
  // 0x7C, /* b */
  // 0x58, /* c */
  // 0x5E, /* d */
  // 0x7B, /* e */
  // 0x71, /* f */
  // 0x6F, /* g */
  // 0x74, /* h */
  // 0x10, /* i */
  // 0x0C, /* j */
  // 0x75, /* k */
  // 0x30, /* l */
  // 0x14, /* m */
  // 0x54, /* n */
  // 0x5C, /* o */
  // 0x73, /* p */
  // 0x67, /* q */
  // 0x50, /* r */
  // 0x6D, /* s */
  // 0x78, /* t */
  // 0x1C, /* u */
  // 0x1C, /* v */
  // 0x14, /* w */
  // 0x76, /* x */
  // 0x6E, /* y */
  // 0x5B, /* z */
  // 0x46, /* { */
  // 0x30, /* | */
  // 0x70, /* } */
  // 0x01, /* ~ */
  // 0x00, /* (del) */
0x0,
0x4C,
0x6,
0xF6,
0xE3,
0x9C,
0xC4,
0x2,
0x23,
0x25,
0x3,
0x92,
0x10,
0x80,
0x8,
0x94,
0x77,
0x44,
0xB5,
0xE5,
0xC6,
0xE3,
0xF3,
0x45,
0xF7,
0xE7,
0x21,
0x61,
0x83,
0xA0,
0x85,
0x9D,
0xF5,
0xD7,
0xF2,
0x33,
0xF4,
0xB3,
0x93,
0x73,
0xD6,
0x12,
0x74,
0xD3,
0x32,
0x51,
0x57,
0x77,
0x97,
0xA7,
0x17,
0xE3,
0xB2,
0x76,
0x76,
0x26,
0xD6,
0xE6,
0xB5,
0x33,
0xC2,
0x65,
0x7,
0x20,
0x4,
0xF5,
0xF2,
0xB0,
0xF4,
0xB7,
0x93,
0xE7,
0xD2,
0x10,
0x60,
0xD3,
0x12,
0x50,
0xD0,
0xF0,
0x97,
0xC7,
0x90,
0xE3,
0xB2,
0x70,
0x70,
0x50,
0xD6,
0xE6,
0xB5,
0xC4,
0x12,
0x92,
0x1,
0x0,


};

#endif
