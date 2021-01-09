#include "i2c.h"

#define NXP_ADDRESS 0x64

unsigned char TunerBufferWrite(unsigned char *buf, uint16_t len) {
  uint16_t i;
  uint8_t r;
  //Serial.print("Send to I2C: ");
  Wire.beginTransmission(NXP_ADDRESS);
  for (i = 0; i < len; i++) {
    Wire.write(buf[i]);
    //Serial.print(buf[i], HEX);
    //Serial.print(" ");
  }
  r = Wire.endTransmission();
  //Serial.print ("- Wire.endTransmission:");
  //Serial.print(r);
  //Serial.println();
  //Serial.println();
  delay(1);
  return (r == 0) ? 1 : 0;
}

unsigned char TunerBufferRead(unsigned char *buf, uint16_t len) {
  uint16_t i;
  //Serial.println("\nRead from I2C: ");
  Wire.requestFrom(NXP_ADDRESS, len);
  if (Wire.available() == len) {
    for (i = 0; i < len; i++) {
      buf[i] = Wire.read();
      //Serial.print(buf[i], HEX);
      //Serial.print(" ");
    }
    //Serial.println();
    return 1;
  }
  return 0;
}

void InitI2C(){
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);

  Wire.begin();
  delay(50);
}