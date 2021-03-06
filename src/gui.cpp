#include "gui.h"

#define TAKESEMAPHORE spiSemaphore != NULL &&xSemaphoreTake(spiSemaphore, (TickType_t)10) == pdTRUE
#define GIVESEMAPHORE xSemaphoreGive(spiSemaphore)

Skin skin;

Gui::Gui() {}

// Init screen
void Gui::InitScreen()
{
  if (TAKESEMAPHORE)
  {
    skin.InitScreen();

    GIVESEMAPHORE;
  }

  display = GUI_RADIOMAIN;
}

// Resets last values if needed, then sets display to main
void Gui::ClearMain(bool clearLastValues)
{
  if (TAKESEMAPHORE)
  {
    if (clearLastValues)
    {
      strcpy(lastFreq, "");
      strcpy(lastRdsPi, "");
      strcpy(lastRdsPiUnstable, "");
      lastRdsPiIsSafe = false;
      strcpy(lastRdsPs, "");
      strcpy(lastRdsRt, "");
      strcpy(lastRdsEcc, "");
      lastRdsFlag = false;
    }

    display = GUI_RADIOMAIN;

    skin.InitMain();
    skin.DisplayRdsFlag(false, true);

    GIVESEMAPHORE;
  }
}

// Reset onscreen RDS informations
void Gui::ClearRds()
{
  DisplayRdsFlag(false);
  DisplayRdsPi(0, 0, 1);
  DisplayRdsPs("", 0);
  DisplayRdsEcc(0, 0, 1);
  DisplayRdsRt("");
}

// Restore last main screen (e.g. leave a menu, close a messagebox, etc.)
void Gui::RestoreLast()
{
  display = GUI_RADIOMAIN;
  if (TAKESEMAPHORE)
  {
    char unit[5];
    strcpy(unit, lastSource == SOURCE_FM ? " MHz" : " kHz");
    skin.InitMain();
    skin.DisplayFreq(lastFreq, unit);
    skin.DisplayRdsPi(lastRdsPiIsSafe ? lastRdsPi : lastRdsPiUnstable, lastRdsPiIsSafe);
    skin.DisplayRdsEcc(lastRdsEcc);
    skin.DisplayRdsPs(lastRdsPsIsSafe ? lastRdsPs : lastRdsPsUnstable, lastRdsPsIsSafe);
    skin.DisplayRdsRt(lastRdsRt);

    GIVESEMAPHORE;
  }
}

// GUI battery display
void Gui::DisplayBattery(uint8_t percentage)
{
  if (TAKESEMAPHORE)
  {
    skin.DisplayBattery(percentage, lastBatteryPercentage);
    lastBatteryPercentage = percentage;

    GIVESEMAPHORE;
  }
}

// GUI source + preset display
void Gui::DisplaySourcePreset(uint8_t source, uint8_t preset)
{
  if (TAKESEMAPHORE)
  {
    skin.DisplaySourcePreset(sourceLUT[source], preset, sourceLUT[lastSource], lastPreset);

    // Save as previous
    lastSource = source;
    lastPreset = preset;

    GIVESEMAPHORE;
  }
}

// GUI RDS flag display
void Gui::DisplayRdsFlag(bool rdsFlag)
{
  if (TAKESEMAPHORE)
  {
    skin.DisplayRdsFlag(rdsFlag, lastRdsFlag);
    lastRdsFlag = rdsFlag;

    GIVESEMAPHORE;
  }
}

// GUI signal display
void Gui::DisplaySignal(int8_t signalValue, int8_t signalMin, int8_t signalMax)
{
  if (TAKESEMAPHORE)
  {
    skin.DisplaySignal(signalValue, signalMin, signalMax, lastSignalValue);
    lastSignalValue = signalValue;

    GIVESEMAPHORE;
  }
}

// GUI frequency display + format and convert to string
void Gui::DisplayFreq(uint16_t freq)
{
  if (TAKESEMAPHORE)
  {
    char buffer[7], unit[5];
    if (lastSource == SOURCE_FM)
    {
      sprintf(buffer, "%.2f", freq / 100.0);
      sprintf(unit, "%s", " MHz");
    }
    else
    {
      sprintf(buffer, "%d", freq);
      sprintf(unit, "%s", " kHz");
    }
    if (display == GUI_RADIOMAIN)
      skin.DisplayFreq(buffer, unit, lastFreq);

    // Save as previous
    strcpy(lastFreq, buffer);

    GIVESEMAPHORE;
  }
}

// GUI RDS PI display + convert to string
void Gui::DisplayRdsPi(uint16_t rdsPi, bool safe, bool forceClear)
{
  if (TAKESEMAPHORE)
  {
    char buffer[5];
    if (forceClear)
      strcpy(buffer, "");
    else
      sprintf(buffer, "%04X", rdsPi);
    if (display == GUI_RADIOMAIN)
      skin.DisplayRdsPi(buffer, safe, lastRdsPiIsSafe ? lastRdsPi : lastRdsPiUnstable, lastRdsPiIsSafe);

    // Save as previous
    strcpy(safe ? lastRdsPi : lastRdsPiUnstable, buffer);
    lastRdsPiIsSafe = safe;

    GIVESEMAPHORE;
  }
}

// GUI RDS ECC display + convert to country string.
// rdsEcc: 8 bit ECC value
// country: 4 bit masked from RDS PI (first hex digit)
void Gui::DisplayRdsEcc(uint8_t rdsEcc, uint8_t country, bool forceClear)
{
  if (TAKESEMAPHORE)
  {
    if (forceClear)
    {
      if (display == GUI_RADIOMAIN)
        skin.DisplayRdsEcc("", lastRdsEcc);
      strcpy(lastRdsEcc, "");
    }
    else
    {
      // Invalid PI country (0xxx) -> exit
      if (!country)
        return;

      // Search ECC country
      uint8_t i = 0;
      while (eccLUT[country - 1][i].Ecc < rdsEcc && i < eccLUTLength[country - 1])
        i++;

      // Final check
      if (eccLUT[country - 1][i].Ecc == rdsEcc)
      {
        // Found: display country
        if (display == GUI_RADIOMAIN)
          skin.DisplayRdsEcc(eccLUT[country - 1][i].Name, lastRdsEcc);
        strcpy(lastRdsEcc, eccLUT[country - 1][i].Name);
      }
    }

    GIVESEMAPHORE;
  }
}

// GUI RDS PS display
// rdsPs: RDS PS <3
// safe: 1: RDS class reported as realible PS, 0: partial of unrealible PS (as reported from RDS class) OR cached PS
void Gui::DisplayRdsPs(const char *rdsPs, bool safe)
{
  if (TAKESEMAPHORE)
  {
    if (display == GUI_RADIOMAIN)
      skin.DisplayRdsPs(rdsPs, safe, lastRdsPsIsSafe ? lastRdsPs : lastRdsPsUnstable, lastRdsPsIsSafe);

    strcpy(safe ? lastRdsPs : lastRdsPsUnstable, rdsPs);
    lastRdsPsIsSafe = safe;

    GIVESEMAPHORE;
  }
}

// GUI RDS RT display
// rdsRt: RDS RT (64 char)
void Gui::DisplayRdsRt(const char *rdsRt)
{
  if (TAKESEMAPHORE)
  {
    if (display == GUI_RADIOMAIN)
      skin.DisplayRdsRt(rdsRt, lastRdsRt);
    strcpy(lastRdsRt, rdsRt);

    GIVESEMAPHORE;
  }
}

/* ENTER FREQ SECTION */

// GUI_ENTERFREQ mode
// Displays enterfreq dialog, formats freq and unit
void Gui::DisplayEnterFreq(uint16_t freq, Sources source)
{
  if (TAKESEMAPHORE)
  {
    char buffer[11];
    strcpy(buffer, source == SOURCE_FM ? "___._ MHz" : source == SOURCE_LW ? "___ kHz" : source == SOURCE_MW ? "____ kHz" : "_____ kHz");

    if (freq > 0)
    {
      uint8_t j = 0;
      char freqStr[6];
      sprintf(freqStr, "%d", freq);

      for (size_t i = 0; i < strlen(freqStr); i++)
      {
        if (i == 0 && freqStr[0] != '1' && (source == SOURCE_FM || source == SOURCE_MW))
        {
          buffer[j++] = ' ';
        }
        if (buffer[j] == '.')
          j++;
        buffer[j++] = freqStr[i];
      }
    }

    skin.DisplayEnterFreq(source == SOURCE_FM ? ENTER_FM : source == SOURCE_LW ? ENTER_LW : source == SOURCE_MW ? ENTER_MW : ENTER_SW, buffer, display != GUI_ENTERFREQ);
    display = GUI_ENTERFREQ;

    GIVESEMAPHORE;
  }
}

/* MESSAGEBOX SECTION */

// GUI_MSGBOX mode
// Displays messagebox
// header: header text of msgbox
// msg: content of msgbox
// time: 0: show eternally, 0>: display time in sec
void Gui::DisplayMsgBox(const char *header, const char *msg, uint8_t time)
{
  if (TAKESEMAPHORE)
  {
    display = GUI_MSGBOX;
    skin.DisplayMsgBox(header, msg);
    GIVESEMAPHORE;
  }
  if (time)
  {
    vTaskDelay(time * 1000 / portTICK_PERIOD_MS);
    RestoreLast();
  }
}

/* MENU SECTION */

// GUI_MENU mode
// Displays menu
// items: array of menu items
// itemcount: array length
// selectedItem: highlighted item's index
void Gui::DisplayMenu(MenuItem items[], uint8_t itemCount, uint8_t selectedItem)
{
  if (TAKESEMAPHORE)
  {
    // Activate menu
    display = GUI_MENU;

    // Set shown menu items range
    if (!selectedItem)
    {
      menuFrom = 0;
      menuTo = MENU_ITEMS - 1;
    }

    while (menuFrom > selectedItem)
    {
      menuFrom--;
      menuTo--;
    }

    while (menuTo < selectedItem)
    {
      menuFrom++;
      menuTo++;
    }

    // Init menu
    char *tmpMenu[itemCount];
    bool tmpChecked[itemCount];
    for (size_t i = 0; i < itemCount; i++)
    {
      tmpMenu[i] = (char *)items[i].Name;
      tmpChecked[i] = items[i].IsChecked;
    }

    // Display menu
    skin.DisplayMenu(tmpMenu, itemCount, selectedItem, tmpChecked, menuFrom, menuTo);

    GIVESEMAPHORE;
  }
}