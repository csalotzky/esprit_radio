#include "interface.h"

Sources tmpSrc;

/* REFS */
Gui *guiRef;
Radio *radioRef;

/* MENU */
MenuView menus[5];
int8_t menuDepth = -1;

void InterfaceInit(Gui *gui, Radio *radio)
{
  guiRef = gui;
  radioRef = radio;
}

// TODO
void PowerOn()
{
  Serial.println("INTERFACE | PowerOn");
}

// Switch off device (deepsleep)
void PowerOff()
{
  Serial.println("INTERFACE | PowerOff");

  // TODO - Should we disable pullups/downs at all??
#ifdef TUNE_ROTARY
  pinMode(ROT1_CLK, INPUT);
  pinMode(ROT1_DT, INPUT);
  pinMode(ROT1_SW, INPUT);
#endif

#ifdef VOL_ROTARY
  pinMode(ROT2_CLK, INPUT);
  pinMode(ROT2_DT, INPUT);
  pinMode(ROT2_SW, INPUT);

#endif

#ifdef NAVI_LEFT
  pinMode(NAVI_LEFT, INPUT);
  pinMode(NAVI_RIGHT, INPUT);
  pinMode(NAVI_UP, INPUT);
  pinMode(NAVI_DOWN, INPUT);
  pinMode(NAVI_OK, INPUT);

#endif
  // radioRef->SwitchSource(SOURCE_STANDBY);

  // esp_deep_sleep_start();
}

// Switch to next source
void PowerSwitch()
{
  Serial.println("INTERFACE | PowerSwitch");

  radioRef->SwitchSource(radioRef->currentStation.Source == SOURCE_BT ? SOURCE_FM : (Sources)(radioRef->currentStation.Source + 1));
}

// Opens menu by default
void Ok()
{
  Serial.println("INTERFACE | Ok");

  radioRef->StopSeek();
  if (guiRef->display != GUI_ENTERFREQ)
  {
    if (menuDepth == -1)
      OpenMenu(MENU_MAIN);
    else if (menus[menuDepth].Items[menus[menuDepth].SelectedItem].IsSubmenu)
      OpenMenu(menus[menuDepth].Items[menus[menuDepth].SelectedItem].SubMenu);
    else if (menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFn != NULL)
      menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFn();
    else if (menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFnInt != NULL)
      menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFnInt(menus[menuDepth].Items[menus[menuDepth].SelectedItem].FnParameter);
    else if (menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFnVarPtrInt != NULL)
      menus[menuDepth].Items[menus[menuDepth].SelectedItem].MenuFnVarPtrInt(menus[menuDepth].Items[menus[menuDepth].SelectedItem].FnVarPtr, menus[menuDepth].Items[menus[menuDepth].SelectedItem].FnParameter);
  }
  else
  {
    radioRef->TuneEnter(0, 1);
    if (menuDepth != -1)
      guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
  }
}

// Exits from menu or enter frequency
void Back()
{
  Serial.println("INTERFACE | Back");

  if (guiRef->display == GUI_MENU)
  {
    if (menuDepth == 0)
      CloseMenu();
    else
    {
      menuDepth--;
      guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
    }
  }
  else if (guiRef->display == GUI_ENTERFREQ)
  {
    radioRef->TuneEnter(-1);
    if (menuDepth != -1)
      guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
  }

  radioRef->StopSeek();
}

// Switch to FM
void SourceFm()
{
  Serial.println("INTERFACE | SourceFm");

  CloseMenu();
  radioRef->StopSeek();
  radioRef->SwitchSource(SOURCE_FM);
}

// Switch to LW
void SourceLw()
{
  Serial.println("INTERFACE | SourceLw");

  CloseMenu();
  radioRef->StopSeek();
  radioRef->SwitchSource(SOURCE_LW);
}

// Switch to MW
void SourceMw()
{
  Serial.println("INTERFACE | SourceMw");

  CloseMenu();
  radioRef->StopSeek();
  radioRef->SwitchSource(SOURCE_MW);
}

// Switch to SW
void SourceSw()
{
  Serial.println("INTERFACE | SourceSw");

  CloseMenu();
  radioRef->StopSeek();
  radioRef->SwitchSource(SOURCE_SW);
}

// TODO
void SourceSwitch()
{
  Serial.println("INTERFACE | SourceSwitch");

  radioRef->StopSeek();
}

// Tune up - fine step
void TuneManualMinstepUp()
{
  Serial.println("INTERFACE | TuneManualMinstepUp");

  radioRef->StopSeek();
  radioRef->TuneManual(1, 1);
}

// Tune down - fine step
void TuneManualMinstepDown()
{
  Serial.println("INTERFACE | TuneManualMinstepDown");

  radioRef->StopSeek();
  radioRef->TuneManual(0, 1);
}

// Tune up - normal step
void TuneManualMaxstepUp()
{
  Serial.println("INTERFACE | TuneManualMaxstepUp");

  radioRef->StopSeek();
  radioRef->TuneManual(1, 0);
}

// Tune down - normal step
void TuneManualMaxstepDown()
{
  Serial.println("INTERFACE | TuneManualMaxstepDown");

  radioRef->StopSeek();
  radioRef->TuneManual(0, 0);
}

// Tune up - step based on config
void TuneManualDefstepUp()
{
  Serial.println("INTERFACE | TuneManualDefstepUp");

  radioRef->StopSeek();
  radioRef->TuneManual(1, radioRef->config.IsMinStep);
}

// Tune down - step based on config
void TuneManualDefstepDown()
{
  Serial.println("INTERFACE | TuneManualDefstepDown");

  radioRef->StopSeek();
  radioRef->TuneManual(0, radioRef->config.IsMinStep);
}

// Seek up
void TuneSeekUp()
{
  Serial.println("INTERFACE | TuneSeekUp");

  radioRef->StopSeek();
  radioRef->TuneSeek(1);
}

// Seek down
void TuneSeekDown()
{
  Serial.println("INTERFACE | TuneSeekDown");

  radioRef->StopSeek();
  radioRef->TuneSeek(0);
}

// Enter frequency dialog - current band
void TuneEnter()
{
  Serial.println("INTERFACE | TuneEnter");

  radioRef->StopSeek();

  if (guiRef->display != GUI_ENTERFREQ)
  {
    radioRef->TuneEnter(0);
    tmpSrc = radioRef->currentStation.Source;
  }
  else
  {
    radioRef->TuneEnter();
    tmpSrc = (tmpSrc >= SOURCE_SW || tmpSrc < SOURCE_FM) ? SOURCE_FM : (Sources)(tmpSrc + 1);
    radioRef->TuneEnterSrc(tmpSrc, 0);
  }
}

// Enter frequency dialog - FM
void TuneEnterFm()
{
  Serial.println("INTERFACE | TuneEnterFm");

  radioRef->StopSeek();

  if (guiRef->display != GUI_ENTERFREQ)
    radioRef->TuneEnterSrc(SOURCE_FM, 0);
  else
    radioRef->TuneEnter();
}

// Enter frequency dialog - LW
void TuneEnterLw()
{
  Serial.println("INTERFACE | TuneEnterLw");

  radioRef->StopSeek();

  if (guiRef->display != GUI_ENTERFREQ)
    radioRef->TuneEnterSrc(SOURCE_LW, 0);
  else
    radioRef->TuneEnter();
}

// Enter frequency dialog - MW
void TuneEnterMw()
{
  Serial.println("INTERFACE | TuneEnterMw");

  radioRef->StopSeek();

  if (guiRef->display != GUI_ENTERFREQ)
    radioRef->TuneEnterSrc(SOURCE_MW, 0);
  else
    radioRef->TuneEnter();
}

// Enter frequency dialog - SW
void TuneEnterSw()
{
  Serial.println("INTERFACE | TuneEnterSw");

  radioRef->StopSeek();

  if (guiRef->display != GUI_ENTERFREQ)
    radioRef->TuneEnterSrc(SOURCE_SW, 0);
  else
    radioRef->TuneEnter();
}

// Menu - Preset lists
void PresetList()
{
  Serial.println("INTERFACE | PresetList");

  Station stations[TOTAL_PRESETS];
  MenuView menu;

  uint8_t j = 0;
  char tmp[TOTAL_PRESETS][50];
  // TODO
  if (ReadAllPresetsSPIFFS(stations) || true)
  {
    for (uint8_t i = 0; i < TOTAL_PRESETS; i++)
    {
      if (stations[i].Source != SOURCE_STANDBY)
      {

        switch (stations[i].Source)
        {
        case SOURCE_FM:
          sprintf(tmp[i], "#%d %s %.2f MHz %s", i + 1, sourceLUT[stations[i].Source], stations[i].Frequency / 100.0, stations[i].RdsPs);
          break;
        case SOURCE_WEB:
          sprintf(tmp[i], "#%d %s %s", i + 1, sourceLUT[stations[i].Source], stations[i].WebName);
          break;
        default:
          sprintf(tmp[i], "#%d %s %d kHz", i + 1, sourceLUT[stations[i].Source], stations[i].Frequency);
          break;
        }

        menu.Items[j] = {tmp[i], 0, 0, MENU_NONE, NULL, RecallPreset, NULL, (uint8_t)(i + 1)};
        if (radioRef->currentPreset == i + 1)
        {
          menu.SelectedItem = j;
          menu.Items[j].IsChecked = true;
        }

        j++;
      }
    }

    menu.ItemsCount = j;
    OpenMenu(menu);
  }
}

// TODO
void VolumeDown()
{
  Serial.println("INTERFACE | VolumeDown");
  radioRef->SetCurrentVolume(false);
}

// TODO
void VolumeUp()
{
  Serial.println("INTERFACE | VolumeUp");
  radioRef->SetCurrentVolume(true);
}

// TODO
void VolumeMute()
{
  Serial.println("INTERFACE | VolumeMute");
}

// TODO
void SetStepMin()
{
  Serial.println("INTERFACE | SetStepMin");
}

// TODO
void SetStepMax()
{
  Serial.println("INTERFACE | SetStepMax");
}

// TODO
void SetStepSwitch()
{
  Serial.println("INTERFACE | SetStepSwitch");

  MenuBool(&radioRef->config.IsMinStep, isMinStepLUT);
}

// Menu - save preset
void SavePreset()
{
  Serial.println("INTERFACE | SavePreset");

  Station stations[TOTAL_PRESETS];
  MenuView menu;
  menu.ItemsCount = TOTAL_PRESETS;

  char tmp[TOTAL_PRESETS][50];
  // TODO
  if (ReadAllPresetsSPIFFS(stations) || true)
  {
    for (size_t i = 0; i < TOTAL_PRESETS; i++)
    {
      Serial.println(stations[i].Source);
      switch (stations[i].Source)
      {
      case SOURCE_FM:
        sprintf(tmp[i], "#%d %s %.2f MHz %s", i + 1, sourceLUT[stations[i].Source], stations[i].Frequency / 100.0, stations[i].RdsPs);
        break;
      case SOURCE_WEB:
        sprintf(tmp[i], "#%d %s %s", i + 1, sourceLUT[stations[i].Source], stations[i].WebName);
        break;
      case SOURCE_STANDBY:
        sprintf(tmp[i], "#%d (EMPTY)", i + 1);
        break;
      default:
        sprintf(tmp[i], "#%d %s %d kHz", i + 1, sourceLUT[stations[i].Source], stations[i].Frequency);
        break;
      }

      menu.Items[i] = {tmp[i], 0, 0, MENU_NONE, NULL, SavePreset, NULL, (uint8_t)(i + 1)};
      if (radioRef->currentPreset == i + 1)
      {
        menu.SelectedItem = i;
        menu.Items[i].IsChecked = true;
      }
    }

    OpenMenu(menu);
  }
}

// Menu - set stereo
void SetStereo()
{
  Serial.println("INTERFACE | SetStereo");

  MenuBool(&radioRef->config.IsStereo, isStereoLUT);
}

// Menu - set region
void SetRegion()
{
  Serial.println("INTERFACE | SetRegion");
  uint8_t regionsLength = sizeof(Regions) / sizeof(Regions[0]);
  uint8_t regionsValues[regionsLength];
  const char *regionsTexts[regionsLength];

  for (size_t i = 0; i < regionsLength; i++)
  {
    regionsValues[i] = i;
    regionsTexts[i] = Regions[i].Name;
  }

  MenuInt(&radioRef->config.Region, regionsValues, regionsTexts, regionsLength);
}

// Menu - set seek signal level
void SetSeekLevel()
{
  Serial.println("INTERFACE | SetSeekLevel");

  uint8_t levelValues[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
  const char *levelTexts[] = {"5 dBuV", "10 dBuV", "15 dBuV", "20 dBuV", "25 dBuV", "30 dBuV", "35 dBuV", "40 dBuV", "45 dBuV", "50 dBuV"};
  MenuInt((uint8_t *)&radioRef->config.SeekLevel, levelValues, levelTexts, 10);
}

// Menu - set seek usn level
void SetSeekUsn()
{
  Serial.println("INTERFACE | SetSeekUsn");

  uint8_t usnValues[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
  const char *usnTexts[] = {"5%", "10%", "15%", "20%", "25%", "30%", "35%", "40%", "45%", "50%"};
  MenuInt((uint8_t *)&radioRef->config.SeekUsn, usnValues, usnTexts, 10);
}

// Menu - set RDS PI mode
void SetPiMode()
{
  Serial.println("INTERFACE | SetPiMode");

  uint8_t values[] = {RDS_REALIBLE, RDS_MIXED, RDS_DX};
  MenuInt((uint8_t *)&radioRef->config.PiMode, values, rdsModeLUT, 3);
}

// Menu - set RDS PS mode
void SetPsMode()
{
  Serial.println("INTERFACE | SetPsMode");

  uint8_t values[] = {RDS_REALIBLE, RDS_MIXED, RDS_DX};
  MenuInt((uint8_t *)&radioRef->config.PsMode, values, rdsModeLUT, 3);
}

// Menu - set RDS AF mode
void SetRdsAf()
{
  Serial.println("INTERFACE | SetPsMode");

  MenuBool(&radioRef->config.IsRdsAf, offOnLUT);
}

// TODO
void SetKeymaps()
{
  Serial.println("INTERFACE | SetKeymaps");
}

/* Methods with parameters */
// Save preset
void SavePreset(uint8_t preset)
{
  Serial.println("INTERFACE | RecallPreset");

  radioRef->SavePreset(preset ? preset : 10);
  CloseMenu();
}

// Switch to preset
void RecallPreset(uint8_t preset)
{
  Serial.println("INTERFACE | RecallPreset");

  radioRef->RecallPreset(preset ? preset : 10);
  CloseMenu();
}

// TODO - Obsolete(?)
void NumberPress(uint8_t number, bool longPress)
{
  Serial.println("INTERFACE | NumberPress");
  if (!longPress)
  {
    if (guiRef->display == GUI_RADIOMAIN)
      RecallPreset(number);
    if (guiRef->display == GUI_ENTERFREQ)
      radioRef->TuneEnter(number);
  }
}

/* ETC */
// Modifies a config variable's value, then saves config and call a reload function in radio class. ptr: pointer to var, value: new value
void SetConfigAttribute(uint8_t *ptr, uint8_t value)
{
  Serial.println("INTERFACE | SetConfigAttribute");
  *ptr = value;
  SaveConfigSPIFFS(radioRef->config);
  radioRef->TunerConfigCommands();

  Back();
}

/* Hardcoded methods */
// KEY_NAVI_LEFT's default action is overrided if menu (or enterfreq dialog) is active.
void MenuLeft()
{
  Serial.println("INTERFACE | MenuLeft");

  if (guiRef->display == GUI_RADIOMAIN || guiRef->display == GUI_RADIOSIGNAL)
    ExecuteButtonDefault(DEFAULT_KEY_LEFT);
  else if (guiRef->display == GUI_MENU || guiRef->display == GUI_ENTERFREQ)
    Back();
}

// KEY_NAVI_RIGHT's default action is overrided if menu (or enterfreq dialog) is active.
void MenuRight()
{
  Serial.println("INTERFACE | MenuRight");

  if (guiRef->display == GUI_RADIOMAIN || guiRef->display == GUI_RADIOSIGNAL)
    ExecuteButtonDefault(DEFAULT_KEY_RIGHT);
  else if (guiRef->display == GUI_MENU || guiRef->display == GUI_ENTERFREQ)
    Ok();
}

// KEY_NAVI_UP's default action is overrided if menu (or enterfreq dialog) is active.
void MenuUp()
{
  Serial.println("INTERFACE | MenuUp");

  if (guiRef->display == GUI_RADIOMAIN || guiRef->display == GUI_RADIOSIGNAL)
    ExecuteButtonDefault(DEFAULT_KEY_UP);
  else if (guiRef->display == GUI_MENU)
  {
    menus[menuDepth].SelectedItem = menus[menuDepth].SelectedItem == 0 ? menus[menuDepth].ItemsCount - 1 : menus[menuDepth].SelectedItem - 1;
    guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
  }
}

// KEY_NAVI_DOWN's default action is overrided if menu (or enterfreq dialog) is active.
void MenuDown()
{
  Serial.println("INTERFACE | MenuDown");

  if (guiRef->display == GUI_RADIOMAIN || guiRef->display == GUI_RADIOSIGNAL)
    ExecuteButtonDefault(DEFAULT_KEY_DOWN);
  else if (guiRef->display == GUI_MENU)
  {
    menus[menuDepth].SelectedItem = menus[menuDepth].SelectedItem + 1 == menus[menuDepth].ItemsCount ? 0 : menus[menuDepth].SelectedItem + 1;
    guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
  }
}

// Executes key's function (based on config)
void ExecuteButtonDefault(uint8_t key, bool longPress)
{
  for (size_t i = 0; i < TOTAL_BUTTONS; i++)
  {
    if (radioRef->config.KeyMaps[i].Key == key && radioRef->config.KeyMaps[i].LongPress == longPress)
    {
      Commands[radioRef->config.KeyMaps[i].CmdIndex].Fn();
      return;
    }
  }
}

/* MENU FUNCTIONS */
// Menu - opens a menu: generates MenuView then send to GUI
void OpenMenu(Menus menu, uint8_t selected)
{
  if (menu == MENU_NONE)
  {
    CloseMenu();
    return;
  }

  menuDepth++;
  uint8_t j = 0;
  // Get and add commands (of current Menus enum)
  for (size_t i = 0; i < sizeof(Commands) / sizeof(Commands[0]); i++)
  {
    if (Commands[i].MenuCat == menu)
    {
      menus[menuDepth].Items[j++] = {Commands[i].DisplayName, 0, 0, MENU_NONE, Commands[i].Fn};
    }
  }
  // Add submenus (only available in main menu)
  if (menu == MENU_MAIN)
  {
    menus[menuDepth].Items[j++] = {"Sources", 0, 1, MENU_SOURCES};
    menus[menuDepth].Items[j++] = {"Tuner settings", 0, 1, MENU_TUNER};
    menus[menuDepth].Items[j++] = {"RDS settings", 0, 1, MENU_RDS};
    // menus[menuDepth].Items[j++] = {"Network settings",0,1,MENU_NETWORK};
    // menus[menuDepth].Items[j++] = {"Bluetooth settings",0,1,MENU_BLUETOOTH};
    menus[menuDepth].Items[j++] = {"Interface settings", 0, 1, MENU_INTERFACE};
  }
  menus[menuDepth].ItemsCount = j;
  menus[menuDepth].SelectedItem = selected;

  // Send to gui
  guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
}

// Menu - opens a menu: MenuView is given as parameter, send to GUI
void OpenMenu(MenuView menuView)
{
  menuDepth++;
  menus[menuDepth] = menuView;
  guiRef->DisplayMenu(menus[menuDepth].Items, menus[menuDepth].ItemsCount, menus[menuDepth].SelectedItem);
}

// Close menu
void CloseMenu()
{
  menuDepth = -1;
  guiRef->RestoreLast();
}

// Menu option for a boolean config varaible
// value: pointer of variable
// displayTexts: true and false labels
void MenuBool(bool *value, const char *displayTexts[])
{
  MenuView menu;
  menu.SelectedItem = *value;
  menu.ItemsCount = 2;

  menu.Items[0] = {(char *)displayTexts[0], !*value, 0, MENU_NONE, NULL, NULL, SetConfigAttribute, 0, (uint8_t *)value};
  menu.Items[1] = {(char *)displayTexts[1], *value, 0, MENU_NONE, NULL, NULL, SetConfigAttribute, 1, (uint8_t *)value};

  OpenMenu(menu);
}

// Menu option for a uint8_t config varaible
// value: pointer of variable
// options: array of available values
// optionsTexts: value labels
// optionsLength: length of options
void MenuInt(uint8_t *value, uint8_t options[], const char *optionsTexts[], uint8_t optionsLength)
{
  MenuView menu;
  menu.ItemsCount = optionsLength;
  for (size_t i = 0; i < menu.ItemsCount; i++)
  {
    menu.Items[i] = {(char *)optionsTexts[i], *value == options[i], 0, MENU_NONE, NULL, NULL, SetConfigAttribute, options[i], (uint8_t *)value};
    if (*value == options[i])
      menu.SelectedItem = i;
  }

  OpenMenu(menu);
}