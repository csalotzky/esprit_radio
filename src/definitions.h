#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

#include "dictionary.h"
#include "Arduino.h"

#define TOTAL_PRESETS 20            // Total station presets
#define TOTAL_BUTTONS 56            // Maximum configurable buttons in keymap function (physical buttons * 2 <= since every button has two states (short and long press))
#define DEFAULT_RDSPIMODE RDS_MIXED // Default RDS PI decode strategy
#define DEFAULT_RDSPSMODE RDS_MIXED // Default RDS PS decode strategy
#define DEFAULT_REGION 0            // Default region (array index)
#define DEFAULT_ISMINSTEP 0         // Default tune step (bool)
#define DEFAULT_ISSTEREO 1          // Default audio mode (bool)
#define DEFAULT_SEEKLEVEL 30        // Default seek signal threshold (dBuV)
#define DEFAULT_SEEKUSN 10          // Default seek usn threshold (%)
#define DEFAULT_ISRDSAF 1           // Default RDS AF mode (bool)

// No-user-modifiable things of seek
#define SEEK_MAX_ATTEMPT 5
#define SEEK_DELAY 5
#define SIGNAL_MIN 0
#define SIGNAL_MAX 70

// Default keymap
#define DEFAULT_KEYMAP                    \
    {                                     \
        {KEY_ROT_ROT1_DOWN, 0, 15},       \
            {KEY_ROT_ROT1_UP, 0, 14},     \
            {KEY_ROT_ROT1_PUSH, 0, 2},    \
            {KEY_ROT_ROT1_PUSH, 1, 3},    \
            {KEY_ROT_ROT2_DOWN, 0, 24},   \
            {KEY_ROT_ROT2_UP, 0, 23},     \
            {KEY_ROT_ROT2_PUSH, 0, 26},   \
            {KEY_ROT_ROT2_PUSH, 1, 29},   \
            {KEY_NAVI_LEFT, 0, 15},       \
            {KEY_NAVI_LEFT, 1, 15},       \
            {KEY_NAVI_RIGHT, 0, 14},      \
            {KEY_NAVI_RIGHT, 1, 14},      \
            {KEY_NAVI_UP, 0, 16},         \
            {KEY_NAVI_DOWN, 0, 17},       \
            {KEY_MATRIX_ASTERISK, 0, 18}, \
            {KEY_MATRIX_SHARP, 0, 19},    \
            {KEY_MATRIX_A, 0, 0},         \
            {KEY_MATRIX_B, 0, 0},         \
            {KEY_MATRIX_C, 0, 0},         \
            {KEY_MATRIX_D, 0, 0},         \
    }

// Hardcoded keys - these buttons's short press event could not be assigned in keymap feature
#define DEFAULT_KEY_PWR_TGL KEY_POWER_SWITCH
#define DEFAULT_KEY_PWR_BTN -1
#define DEFAULT_KEY_OK KEY_ROT_ROT1_PUSH
#define DEFAULT_KEY_BACK KEY_ROT_ROT2_PUSH
#define DEFAULT_KEY_LEFT KEY_ROT_ROT2_DOWN
#define DEFAULT_KEY_RIGHT KEY_ROT_ROT2_UP
#define DEFAULT_KEY_UP KEY_ROT_ROT1_UP
#define DEFAULT_KEY_DOWN KEY_ROT_ROT1_DOWN

// Function pointers used in menu struct
typedef void (*pCommandFn)();
typedef void (*pCommandFnInt)(uint8_t);
typedef void (*pCommandFnPtrInt)(uint8_t *, uint8_t);

// Available hardware buttons
enum Keys
{
    KEY_POWER_BTN = 1,
    KEY_POWER_SWITCH,
    KEY_ROT_ROT1_DOWN,
    KEY_ROT_ROT1_UP,
    KEY_ROT_ROT1_PUSH,

    KEY_ROT_ROT2_DOWN,
    KEY_ROT_ROT2_UP,
    KEY_ROT_ROT2_PUSH,

    KEY_NAVI_LEFT,
    KEY_NAVI_RIGHT,
    KEY_NAVI_UP,
    KEY_NAVI_DOWN,
    KEY_NAVI_OK,

    KEY_MATRIX_1,
    KEY_MATRIX_2,
    KEY_MATRIX_3,
    KEY_MATRIX_4,
    KEY_MATRIX_5,
    KEY_MATRIX_6,
    KEY_MATRIX_7,
    KEY_MATRIX_8,
    KEY_MATRIX_9,
    KEY_MATRIX_0,
    KEY_MATRIX_ASTERISK,
    KEY_MATRIX_SHARP,
    KEY_MATRIX_A,
    KEY_MATRIX_B,
    KEY_MATRIX_C,
    KEY_MATRIX_D,
};

/* ENUMS */
enum Sources
{
    SOURCE_STANDBY,
    SOURCE_FM,
    SOURCE_LW,
    SOURCE_MW,
    SOURCE_SW,
    SOURCE_WEB,
    SOURCE_BT
};

enum GuiDisplay
{
    GUI_STANDBY,
    GUI_RADIOMAIN,
    GUI_RADIOSIGNAL,
    GUI_ENTERFREQ,
    GUI_MENU,
    GUI_MSGBOX
};

enum RdsModes
{
    RDS_REALIBLE,
    RDS_MIXED,
    RDS_DX
};

enum Menus
{
    MENU_NONE,
    MENU_MAIN,
    MENU_SOURCES,
    MENU_RDS,
    MENU_TUNER,
    MENU_NETWORK,
    MENU_BLUETOOTH,
    MENU_INTERFACE
};

enum SeekSates
{
    SEEK_DOWN = -1,
    SEEK_STOP = 0,
    SEEK_UP = 1
};

/* STRUCTS */
// TODO: OIRT steps
const struct
{
    char Name[7];
    uint16_t FmMin;
    uint16_t FmMax;
    uint8_t FmMinStep;
    uint8_t FmMaxStep;
    uint16_t LwMin;
    uint16_t LwMax;
    uint8_t LwMinStep;
    uint8_t LwMaxStep;
    uint16_t MwMin;
    uint16_t MwMax;
    uint8_t MwMinStep;
    uint8_t MwMaxStep;
    uint16_t SwMin;
    uint16_t SwMax;
    uint8_t SwMinStep;
    uint8_t SwMaxStep;
} Regions[] = {
    //  Reg. name   FM range        FM steps    LW range    LW steps    MW range    MW steps    SW steps        SW range
    {"DX", 6500, 10800, 1, 10, 153, 521, 1, 9, 522, 2299, 1, 9, 2300, 27000, 1, 5},
    {"DX USA", 6500, 10800, 1, 10, 150, 519, 1, 10, 520, 2299, 1, 10, 2300, 27000, 1, 5},
    {"EUR", 8750, 10800, 5, 10, 153, 279, 1, 9, 522, 1701, 1, 9, 2300, 27000, 1, 5},
    {"USA", 8750, 10800, 10, 20, 153, 279, 1, 9, 520, 1700, 1, 9, 2300, 27000, 1, 5},
    {"JPN", 7600, 9000, 1, 10, 153, 279, 1, 9, 522, 1701, 1, 9, 2300, 27000, 1, 5},
};

struct Station
{
    Sources Source = SOURCE_STANDBY;
    uint16_t Frequency = 0;
    uint16_t RdsPi = 0;
    uint8_t RdsEcc = 0;
    char RdsPs[9] = "";
    char WebName[33] = "";
    char WebUuid[33] = "";
};

struct KeyMap
{
    uint8_t Key;
    bool LongPress;
    uint8_t CmdIndex;
};

struct Config
{
    RdsModes PsMode = DEFAULT_RDSPSMODE;
    RdsModes PiMode = DEFAULT_RDSPIMODE;
    uint8_t Region = DEFAULT_REGION;
    bool IsMinStep = DEFAULT_ISMINSTEP;
    bool IsStereo = DEFAULT_ISSTEREO;
    uint8_t SeekLevel = DEFAULT_SEEKLEVEL;
    uint8_t SeekUsn = DEFAULT_SEEKUSN;
    bool IsRdsAf = DEFAULT_ISRDSAF;

    KeyMap KeyMaps[TOTAL_BUTTONS * 2] = DEFAULT_KEYMAP;
    bool IsNumKeyDefaultPreset = true;
};

struct MenuItem
{
    const char *Name;
    bool IsChecked;
    bool IsSubmenu;
    Menus SubMenu;

    pCommandFn MenuFn;
    pCommandFnInt MenuFnInt;
    pCommandFnPtrInt MenuFnVarPtrInt;

    uint8_t FnParameter;
    uint8_t *FnVarPtr;
};

#endif