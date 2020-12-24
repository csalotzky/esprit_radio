#ifndef I2C_h
#define I2C_h

#include <Wire.h> 
#include <Arduino.h>

unsigned char TunerBufferWrite(unsigned char *buf, uint16_t len);
unsigned char TunerBufferRead(unsigned char *buf, uint16_t len);
void InitI2C();

#endif
