#ifndef _TASKS_H_
#define _TASKS_H_

#include "Arduino.h"

extern SemaphoreHandle_t i2cSemaphore;
extern SemaphoreHandle_t spiSemaphore;

void InitSemaphore();

#endif