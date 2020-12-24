#ifndef _INPUTS_H_
#define _INPUTS_H_

#include "i2ckeypad.h"
#include <Keypad.h>        // GDY120705
#include <Wire.h>
#include "radio.h"
#include "gui.h"
#include "interface.h"
#include "device.h"

void InputsInit(Gui *gui, Radio *radio);
void KeypadLoop(void* params);
void ButtonLoop(void* params);

void InputButtonNumbers(uint8_t num);
void InputButtonA();
void InputButtonB();
void InputButtonC();
void InputButtonD();
void InputButtonAsterisk();
void InputButtonSharp();

#endif
