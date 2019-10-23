// Arduino Uno Programmer for AT28 EEPROM
// Chris Torrence, 2015
// ideas from:  http://forum.6502.org/viewtopic.php?f=4&t=2491
//

#include "ProgrammerAT28.h"


void setup()
{
  Serial.begin(115200);

  setupAT28();

  Serial.println("***** TESTING *****");
  testShiftRegister();

  // Wait until user hits return.
  if (1) {
    Serial.println("***** Hit <RETURN> to start WRITE/VERIFY *****");
    while (Serial.available() == 0) {};
    Serial.read();
    int success = writeEEPROM();
    if (success) {
      verifyEEPROM();
    }
  } else {
    Serial.println("***** Hit <RETURN> to read & dump EEPROM *****");
    while (Serial.available() == 0) {};
    Serial.read();
    readEEPROM();
  }
}


void loop()
{
  // Do nothing
}
