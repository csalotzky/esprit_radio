#ifndef SkinTTGO_h
#define SkinTTGO_h

#define MENU_ITEMS      5

#include <TFT_eSPI.h>
#include <SPI.h>
#include "rdsLogo.h"


class Skin {
  public:
    /* INIT METHODS */
    Skin();
    void InitScreen();
    void InitMain();
    
    /* RADIO RELATED */
    void DisplayBattery(int8_t currentPercentage, int8_t lastPercentage = -1);
    void DisplaySourcePreset(const char* currentSource, uint8_t currentPreset, const char* lastSource = "", uint8_t lastPreset = 0);
    void DisplayRdsFlag(bool currentRdsFlag, bool lastRdsFlag = false);
    void DisplaySignal(int8_t currentSignalValue, int8_t signalMin, int8_t signalMax, int8_t lastSignalValue = 0);
    void DisplayFreq(const char* currentFrequency, const char* freqUnit, const char* lastFrequency = "");
    void DisplayRdsPi(const char* currentRdsPi, bool currentSafe, const char* lastRdsPi = "", bool lastSafe = false);
    void DisplayRdsEcc(const char* currentCountry, const char* lastCountry = "");
    void DisplayRdsPs(const char* currentRdsPs, bool currentSafe, const char* lastRdsPs = "", bool lastSafe = false); 
    void DisplayRdsRt(const char* currentRdsRt, const char* lastRdsRt = ""); 

    /* ENTER FREQUENCY DIALOG */
    void DisplayEnterFreq(const char* header, const char* freqString, bool drawFrame = true);
    
    /* MESSAGEBOX */
    void DisplayMsgBox(const char* header, const char* text, uint8_t headerFont=4, uint8_t textFont=2);

    /* MENU */
    void DisplayMenu(char* items[], uint8_t itemCount, uint8_t selectedItem, bool checkedItems[], uint8_t itemsFrom, uint8_t itemsTo);
};

#endif