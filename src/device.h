#ifndef __DEVICE_H
#define __DEVICE_H

#include "Arduino.h"
#include <EasyButton.h>
#include <EasyButtonVirtual.h>
#include "definitions.h"

/* Enabled button groups */
//#define NAVI_BUTTONS
#define I2C_KEYPAD
#define VOL_ROTARY
#define TUNE_ROTARY
//#define POWER_SWITCH

/* Button pinouts */
#define ROT1_CLK 14 // 36
#define ROT1_DT 27  // 37
#define ROT1_SW 26  // 38
#define ROT2_CLK 25 // 33
#define ROT2_DT 33  // 32
#define ROT2_SW 32  // 39

#define KEYPAD_I2C_ADDR 0x20

//#define NAVI_LEFT 2
//#define NAVI_RIGHT 12
//#define NAVI_UP 17
//#define NAVI_DOWN 15
//#define NAVI_OK 13

//#define POWER_SWITCH_PIN -1

/* HW EasyButton */
struct HardwareButton
{
    uint8_t KeyId;
    EasyButton Btn;
    pCommandFn RepeatableLongFn; // Short press and simple long press functions stored in EasyButton object. This stores the callback function only if long press requires to execute command repeatedly. Not the nicest solution tho
};

/* Virtual EasyButton (rotary+matrix) */
struct VirtualButton
{
    uint8_t KeyId;
    EasyButtonVirtual Btn;
    pCommandFn RepeatableLongFn; // Short press and simple long press functions stored in EasyButton object. This stores the callback function only if long press requires to execute command repeatedly. Not the nicest solution tho
};

/* Stores a rotary encoders states */
struct Rotary
{
    int8_t Clk;
    int8_t Dt;
    // uint8_t Sw;
    uint8_t lastStateClk;
    // unsigned long lastButtonPress;
};

/* Externs */
extern bool state_KEY_MATRIX_A, state_KEY_MATRIX_B, state_KEY_MATRIX_C, state_KEY_MATRIX_D, state_KEY_MATRIX_ASTERISK, state_KEY_MATRIX_SHARP, state_KEY_MATRIX_NUM,
    state_KEY_ROT_ROT1_DOWN, state_KEY_ROT_ROT1_UP, state_KEY_ROT_ROT2_DOWN, state_KEY_ROT_ROT2_UP;

extern Rotary rotTune;
extern Rotary rotVol;

extern HardwareButton HwButtons[];
extern VirtualButton VirtButtonsMatrix[];
extern VirtualButton VirtButtonsRotary[];

extern size_t HwButtonsSize;
extern size_t VirtButtonsMatrixSize;
extern size_t VirtButtonsRotarySize;

#endif