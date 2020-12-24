#ifndef TUNER_BOOT_
#define TUNER_BOOT_

#include "Arduino.h"
#include "i2c.h"

void Tuner_WaitMs (unsigned int ms);
uint16_t Tuner_Init();

#endif