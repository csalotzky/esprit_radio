#ifndef _INPUTS_H_
#define _INPUTS_H_

//#include "i2ckeypad.h"
#include <Keypad.h>
#include <Wire.h>
#include "radio.h"
#include "gui.h"
#include "interface.h"
#include "device.h"

void InputsInit(Gui *gui, Radio *radio);
void KeypadLoop(void* params);
void ButtonLoop(void* params);

#endif