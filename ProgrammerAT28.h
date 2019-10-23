#define SER_Pin   A3  // 75HC595 pin 14, serial input
#define STCP_Pin  A2  // 75HC595 pin 12, STCP storage register clock
#define SHCP_Pin  A1  // 75HC595 pin 11, SHCP shift register clock

#define AT28_CE_Pin  10  // AT28 CE, chip enable
#define AT28_WE_Pin  11  // AT28 WE, write enable
#define AT28_OE_Pin  12  // AT28 OE, output enable

#define numOfRegisterPins 16

// Fast way of setting/reading/writing pins on the Arduino
// http://masteringarduino.blogspot.com/2013_10_01_archive.html
#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P) ((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)  ((uint8_t)(1<<pinIndex(P)))
#define digitalLow(P)  *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P) ((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)  ((*(pinOfPin(P))& pinMask(P))==0)

void testShiftRegister();
int writeEEPROM();
void verifyEEPROM();
void readEEPROM();
void setupAT28();
