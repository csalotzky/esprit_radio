#ifndef _GUI_H_
#define _GUI_H_

#include "skinTTGO.h"
#include "definitions.h"
#include "tasks.h"
#include "Arduino.h"

#include <stdarg.h>

class Gui {
  public:
    Gui();
    void InitScreen();
    /* RADIO MAIN SECTION */
    void ClearMain(bool clearLastValues = false);
    void ClearRds();
    void RestoreLast();
    void DisplayBattery(uint8_t percentage);
    void DisplaySourcePreset(uint8_t source, uint8_t preset);
    void DisplayRdsFlag(bool rdsFlag);
    void DisplaySignal(int8_t signalValue, int8_t signalMin, int8_t signalMax);
    void DisplayFreq(uint16_t freq);
    void DisplayRdsPi(uint16_t rdsPi, bool safe, bool forceClear = false);
    void DisplayRdsEcc(uint8_t rdsEcc, uint8_t country, bool forceClear = false);
    void DisplayRdsPs(const char* rdsPs, bool safe); 
    void DisplayRdsRt(const char* rdsRt);
    /* ENTER FREQ SECTION */
    void DisplayEnterFreq(uint16_t freq, Sources source);
    /* MESSAGEBOX SECTION */
    void DisplayMsgBox(const char* header, const char* msg, uint8_t time=0);
    /* MENU SECTION */
    void DisplayMenu(MenuItem items[], uint8_t itemCount, uint8_t selectedItem);

    /* GUI */
    GuiDisplay display;

  private:
    /* SOURCE */
    uint8_t lastSource;
    uint8_t lastPreset;
    /* RADIO */
    char lastFreq[8];
    bool lastRdsFlag;

    char lastRdsPs[9];
    char lastRdsPsUnstable[9];
    bool lastRdsPsIsSafe; 
    
    char lastRdsPi[5];
    char lastRdsPiUnstable[5];
    bool lastRdsPiIsSafe;
    
    char lastRdsEcc[14];
    char lastRdsRt[65];
    int8_t lastBatteryPercentage = -1;
    int8_t lastSignalValue;

    uint8_t menuFrom;
    uint8_t menuTo;
};

#endif
