#include "device.h"

#ifdef I2C_KEYPAD
bool state_KEY_MATRIX_A, state_KEY_MATRIX_B, state_KEY_MATRIX_C, state_KEY_MATRIX_D, state_KEY_MATRIX_ASTERISK, state_KEY_MATRIX_SHARP, state_KEY_MATRIX_NUM;
#endif

#ifdef TUNE_ROTARY
Rotary rotTune = {ROT1_CLK,ROT1_DT};
bool state_KEY_ROT_ROT1_DOWN, state_KEY_ROT_ROT1_UP;
#endif

#ifdef VOL_ROTARY 
Rotary rotVol = {ROT2_CLK,ROT2_DT};
bool state_KEY_ROT_ROT2_DOWN, state_KEY_ROT_ROT2_UP;
#endif

HardwareButton HwButtons[] = {
    #ifdef TUNE_ROTARY
    { KEY_ROT_ROT1_PUSH,EasyButton(ROT1_SW) },
    #endif

    #ifdef VOL_ROTARY
    { KEY_ROT_ROT2_PUSH,EasyButton(ROT2_SW) },
    #endif

    #ifdef NAVI_BUTTONS
    { KEY_NAVI_LEFT,EasyButton(NAVI_LEFT) },
    { KEY_NAVI_RIGHT,EasyButton(NAVI_RIGHT) },
    { KEY_NAVI_UP,EasyButton(NAVI_UP) },
    { KEY_NAVI_DOWN,EasyButton(NAVI_DOWN) },
    { KEY_NAVI_OK,EasyButton(NAVI_OK) },
    #endif

    #ifdef POWER_SWITCH
    { KEY_POWER_SWITCH,EasyButton(POWER_SWITCH_PIN) },
    #endif
};

VirtualButton VirtButtonsMatrix[] = {
    #ifdef I2C_KEYPAD
    { KEY_MATRIX_A,EasyButtonVirtual(state_KEY_MATRIX_A) },
    { KEY_MATRIX_B,EasyButtonVirtual(state_KEY_MATRIX_B) },
    { KEY_MATRIX_C,EasyButtonVirtual(state_KEY_MATRIX_C) },
    { KEY_MATRIX_D,EasyButtonVirtual(state_KEY_MATRIX_D) },
    { KEY_MATRIX_ASTERISK,EasyButtonVirtual(state_KEY_MATRIX_ASTERISK) },
    { KEY_MATRIX_SHARP,EasyButtonVirtual(state_KEY_MATRIX_SHARP) },
    { KEY_MATRIX_1, EasyButtonVirtual(state_KEY_MATRIX_NUM) }
    #endif
};

VirtualButton VirtButtonsRotary[] = {
    #ifdef VOL_ROTARY
    { KEY_ROT_ROT1_DOWN,EasyButtonVirtual(state_KEY_ROT_ROT1_DOWN) },
    { KEY_ROT_ROT1_UP,EasyButtonVirtual(state_KEY_ROT_ROT1_UP) },
    #endif

    #ifdef TUNE_ROTARY
    { KEY_ROT_ROT2_DOWN,EasyButtonVirtual(state_KEY_ROT_ROT2_DOWN) },
    { KEY_ROT_ROT2_UP,EasyButtonVirtual(state_KEY_ROT_ROT2_UP) },
    #endif
};

#if defined(TUNE_ROTARY) || defined(VOL_ROTARY) || defined(NAVI_BUTTONS) || defined(POWER_SWITCH)
size_t HwButtonsSize = sizeof(HwButtons)/sizeof(HwButtons[0]);
#else
size_t HwButtonsSize = 0;
#endif

#ifdef I2C_KEYPAD
size_t VirtButtonsMatrixSize = sizeof(VirtButtonsMatrix)/sizeof(VirtButtonsMatrix[0]);
#else
size_t VirtButtonsMatrixSize = 0;
#endif

#if  defined(TUNE_ROTARY) || defined(VOL_ROTARY)
size_t VirtButtonsRotarySize = sizeof(VirtButtonsRotary)/sizeof(VirtButtonsRotary[0]);
#else
size_t VirtButtonsRotarySize = 0;
#endif