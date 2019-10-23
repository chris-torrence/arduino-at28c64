
#include <Arduino.h>
#include "ProgrammerAT28.h"

#include "ProgData.h"


// Procedure to set and display registers:
//   STCP low
//   For each address bit (high to low): SHCP low, write bit, SHCP high
//   STCP high to transfer to the storage register (and the output)
//
void writeShiftRegister(long val)
{
  digitalWrite(STCP_Pin, LOW);

  // From highest bit to lowest bit
  for(int i = 15; i >=  0; i--)
  {
    digitalWrite(SHCP_Pin, LOW);
    digitalWrite(SER_Pin, (val >> i) & 1);  // Write the next highest bit
    digitalWrite(SHCP_Pin, HIGH);  // Left shift the register 1 bit

  }
  digitalWrite(STCP_Pin, HIGH);
}


// Light up the LEDs using each valid address
void testShiftRegister()
{
  // Just light up the LEDs in order.
  writeShiftRegister(0);
  delay(100);
  for (int i=0; i < numOfRegisterPins; i++)
  {
    writeShiftRegister(1 << i);
    delay(50);
  }
  // Light them all up, then turn them off.
  writeShiftRegister(8191);
  delay(50);
  writeShiftRegister(0);
}


void printValue(int address, byte value, int debug)
{
  if (address % 32 == 0 || debug)
  {
    Serial.println("");
    Serial.print("$");
    if (address < 4096) Serial.print("0");
    if (address < 256) Serial.print("0");
    if (address < 16) Serial.print("0");
    Serial.print(address, HEX);
    Serial.print(":");
  }

  Serial.print(" ");
  if (value < 16) Serial.print("0");
  Serial.print(value, HEX);
}

byte readValue(int address)
{
  // Now read the value.
  // Set the AT28 for "READ", WE high, OE low
  // Set the data pins for input
  digitalWrite(AT28_WE_Pin, HIGH);
  for (int i=2; i <= 9; i++) pinMode(i, INPUT);

  writeShiftRegister(address);
  digitalWrite(AT28_OE_Pin, LOW);
  
  int actualValue = 0;
  // the (9 - i) translates from bit # to pin #
  for (int i=0; i <= 7; i++) {
    if (digitalRead(9 - i)) actualValue += (1 << i);
  }

  digitalWrite(AT28_OE_Pin, HIGH);
  return actualValue;
}


void writeSingleByte(long address, byte value)
{
    digitalWrite(AT28_OE_Pin, HIGH);
    digitalWrite(AT28_WE_Pin, HIGH);
    writeShiftRegister(address);

    // the (9 - i) translates from bit # to pin #
    for (int i = 0; i <= 7; i++)
    {
      if (value & (1 << i))
        digitalHigh(9 - i);
      else
        digitalLow(9 - i);
    }

    // Send a pulse to the AT28 to write the data
    digitalLow(AT28_WE_Pin);
    delayMicroseconds(20);
    digitalHigh(AT28_WE_Pin);
    delayMicroseconds(100);
}


void disableDataProtect() {
  writeSingleByte(0x1555, 0xAA);
  writeSingleByte(0x0AAA, 0x55);
  writeSingleByte(0x1555, 0x80);
  writeSingleByte(0x1555, 0xAA);
  writeSingleByte(0x0AAA, 0x55);
  writeSingleByte(0x1555, 0x20);
  delay(20);
}


void enableDataProtect()
{
  delay(20);
  writeSingleByte(0x1555, 0xAA);
  writeSingleByte(0x0AAA, 0x55);
  writeSingleByte(0x1555, 0xA0);
}


// Procedure to write to the AT28:
//   Set OE and WE high
//   For each address, write the address and data, then pulse WE low
int writeEEPROM()
{
  Serial.println("");
  Serial.println("***** WRITE *****");
  digitalWrite(AT28_CE_Pin, LOW);

  for (int i = 2; i <= 9; i++) {
    pinMode(i, OUTPUT);
  }

//  disableDataProtect();

  byte currentValue;

  for (int address = 0; address < ADDRESS_MAX; address++)
  {
    byte value = pgm_read_byte_near(values + address);
    printValue(address, value, 0);
    writeSingleByte(address, value);
  }

//  enableDataProtect();
  writeShiftRegister(0);
  return 1;
  }


  // Procedure to read from the AT28:
  //   Set OE low and WE high
  //   For each address, write the address, then read the data
  void verifyEEPROM()
  {
    Serial.println("");
    Serial.println("***** READ *****");
    for (int i = 2; i <= 9; i++)
      pinMode(i, INPUT);
    digitalWrite(AT28_CE_Pin, LOW);

    for (int address = 0; address < ADDRESS_MAX; address++)
    {
      int value = readValue(address);

      byte expectvalue = pgm_read_byte_near(values + address);

      if (expectvalue != value)
      {
        printValue(address, value, 1);
        Serial.print(" <-- Wrong value, should be: ");
        if (expectvalue < 16)
          Serial.print("0");
        Serial.println(expectvalue, HEX);
      }
      else
      {
        printValue(address, value, 0);
      }
    }

    writeShiftRegister(0);
    Serial.println("");
  }


  // Procedure to read from the AT28:
  //   Set OE low and WE high
  //   For each address, write the address, then read the data
  void readEEPROM()
  {
    Serial.println("");
    Serial.println("***** READ *****");
    for (int i = 2; i <= 9; i++)
      pinMode(i, INPUT);
    digitalWrite(AT28_CE_Pin, LOW);

    for (int address = 0; address < ADDRESS_MAX; address++)
    {
      if (address % 32 == 0)
        Serial.println("");
      int value = readValue(address);
      Serial.print(value);
      Serial.print(",");

      delay(1);
    }

    writeShiftRegister(0);
    Serial.println("");
  }

  // Initial setup of all of the AT28C16/C64 pins.
  void setupAT28()
  {

    pinMode(SER_Pin, OUTPUT);
    pinMode(STCP_Pin, OUTPUT);
    pinMode(SHCP_Pin, OUTPUT);

    pinMode(AT28_CE_Pin, OUTPUT);
    pinMode(AT28_WE_Pin, OUTPUT);
    pinMode(AT28_OE_Pin, OUTPUT);

    // Disable the AT28
    digitalWrite(AT28_CE_Pin, HIGH);
    digitalWrite(AT28_WE_Pin, HIGH);
    digitalWrite(AT28_OE_Pin, HIGH);

    for (int i = 2; i <= 9; i++)
      pinMode(i, INPUT);

    if (ADDRESS_MAX > 2048)
    {
      Serial.println("***** Is the switch in the UP position?! *****");
    }
    else
    {
      Serial.println("***** Is the switch in the DOWN position?! *****");
    }
  }
