#ifndef __DICTIONARY_H
#define __DICTIONARY_H

#include "Arduino.h"

#define ENTER_FM    "ENTER FM FREQUENCY: "
#define ENTER_LW    "ENTER LW FREQENCY: "
#define ENTER_MW    "ENTER MW FREQUENCY: "
#define ENTER_SW    "ENTER SW FREQUENCY: "

#define ERROR_HEADER        "ERROR!"
#define ERROR_INVALIDFREQ   "Invalid frequency"
#define ERROR_INVALIDPRESET "Invalid preset"
#define MSG_PRESETSAVED     "Preset saved!"

struct Country {
    const char* Name;
    const char* Itu;
    uint8_t Ecc;
};

extern const char* sourceLUT[];
extern const char* isMinStepLUT[];
extern const char* isStereoLUT[];
extern const char* noYesLUT[];
extern const char* offOnLUT[];
extern const char* rdsModeLUT[];
extern const Country* eccLUT[15];

#endif