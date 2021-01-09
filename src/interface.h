#ifndef Interface_h
#define Interface_h

#include "gui.h"
#include "radio.h"
#include "definitions.h"
#include "filetasks.h"
#include "device.h"

void InterfaceInit(Gui *gui, Radio *radio);

void PowerOn();
void PowerOff();
void PowerSwitch();
void Ok();
void Back();
void SourceFm();
void SourceLw();
void SourceMw();
void SourceSw();
void SourceSwitch();
void TuneManualMinstepUp();
void TuneManualMinstepDown();
void TuneManualMaxstepUp();
void TuneManualMaxstepDown();
void TuneManualDefstepUp();
void TuneManualDefstepDown();
void TuneSeekUp();
void TuneSeekDown();
void TuneEnter();
void TuneEnterFm();
void TuneEnterLw();
void TuneEnterMw();
void TuneEnterSw();
void PresetList();
void VolumeDown();
void VolumeUp();
void VolumeMute();
void SetStepMin();
void SetStepMax();
void SetStepSwitch();
void SavePreset();
void SetStereo();
void SetRegion();
void SetSeekLevel();
void SetSeekUsn();
void SetPiMode();
void SetPsMode();
void SetRdsAf();
void SetKeymaps();

void SavePreset(uint8_t preset);
void RecallPreset(uint8_t preset);
void NumberPress(uint8_t number);

void SetConfigAttribute(uint8_t *ptr, uint8_t value);

void ExecuteButtonDefault(uint8_t key, bool longPress = false);
void MenuLeft();
void MenuRight();
void MenuUp();
void MenuDown();


struct MenuView {
  MenuItem Items[30];
  uint8_t ItemsCount;
  uint8_t SelectedItem = 0;
};

void OpenMenu(Menus menu, uint8_t selected = 0);
void OpenMenu(MenuView menuView);
void CloseMenu();

void MenuBool(bool *value, const char *displayTexts[]);
void MenuInt(uint8_t *value, uint8_t options[], const char *optionsTexts[], uint8_t optionsLength);

const struct {
  const char *DisplayName;
  Menus MenuCat;
  bool EnableAssign;
  bool RepeatableExecute;
  pCommandFn Fn;
} Commands[] = {
    //Disp. name                        Menu category   Can assign to btn   RepeatableExecute Function pointer  
    {"Power On",                        MENU_NONE,      1,                  0,                &PowerOn},
    {"Power Off",                       MENU_NONE,      1,                  0,                &PowerOff},
    {"Power Switch",                    MENU_NONE,      1,                  0,                &PowerSwitch},
    {"Ok",                              MENU_NONE,      1,                  0,                &Ok},
    {"Back",                            MENU_NONE,      1,                  0,                &Back},
    {"FM",                              MENU_SOURCES,   1,                  0,                &SourceFm},
    {"LW",                              MENU_SOURCES,   1,                  0,                &SourceLw},
    {"MW",                              MENU_SOURCES,   1,                  0,                &SourceMw},
    {"SW",                              MENU_SOURCES,   1,                  0,                &SourceSw},
    {"Switch source",                   MENU_NONE,      1,                  0,                &SourceSwitch},
    {"Tune Up (fine)",                  MENU_NONE,      1,                  1,                &TuneManualMinstepUp},
    {"Tune Down (fine)",                MENU_NONE,      1,                  1,                &TuneManualMinstepDown},
    {"Tune Up (normal)",                MENU_NONE,      1,                  1,                &TuneManualMaxstepUp},
    {"Tune Down (normal)",              MENU_NONE,      1,                  1,                &TuneManualMaxstepDown},
    {"Tune Up (current)",               MENU_NONE,      1,                  1,                &TuneManualDefstepUp},
    {"Tune Down (current)",             MENU_NONE,      1,                  1,                &TuneManualDefstepDown},
    {"Search Up",                       MENU_NONE,      1,                  0,                &TuneSeekUp},
    {"Search Down",                     MENU_NONE,      1,                  0,                &TuneSeekDown},
    {"Enter frequency",                 MENU_NONE,      0,                  0,                &TuneEnter},
    {"Enter frequency (FM)",            MENU_NONE,      0,                  0,                &TuneEnterFm},
    {"Enter frequency (LW)",            MENU_NONE,      0,                  0,                &TuneEnterLw},
    {"Enter frequency (MW)",            MENU_NONE,      0,                  0,                &TuneEnterMw},
    {"Enter frequency (SW)",            MENU_NONE,      0,                  0,                &TuneEnterSw},
    {"Volume Up",                       MENU_NONE,      0,                  1,                &VolumeUp},
    {"Volume Down",                     MENU_NONE,      1,                  1,                &VolumeDown},
    {"Mute",                            MENU_NONE,      1,                  0,                &VolumeMute},
    {"Set step size: Fine",             MENU_NONE,      1,                  0,                &SetStepMin},
    {"Set step size: Normal",           MENU_NONE,      1,                  0,                &SetStepMax},
    {"Switch step size",                MENU_TUNER,     1,                  0,                &SetStepSwitch},
    {"Presets",                         MENU_MAIN,      1,                  0,                &PresetList},
    {"Save preset",                     MENU_MAIN,      1,                  0,                &SavePreset},
    {"Stereo mode",                     MENU_TUNER,     1,                  0,                &SetStereo},
    {"Region",                          MENU_TUNER,     1,                  0,                &SetRegion},
    {"Seek level threshold",            MENU_TUNER,     1,                  0,                &SetSeekLevel},
    {"Seek USN threshold",              MENU_TUNER,     1,                  0,                &SetSeekUsn},
    {"PI decode method",                MENU_RDS,       1,                  0,                &SetPiMode},
    {"PS decode method",                MENU_RDS,       1,                  0,                &SetPsMode},
    //{"AF",                              MENU_RDS,       1,                  0,                &SetRdsAf},
    {"Set keymaps",                     MENU_INTERFACE, 0,                  0,                &SetKeymaps},
};

//extern const Command Commands[];
extern Gui *guiRef;
extern Radio *radioRef;

#endif