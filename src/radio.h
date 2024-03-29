#ifndef Radio_h
#define Radio_h

#include "TEF668X/TEF668X.h"
#include "gui.h"
#include "definitions.h"
#include "rds.h"
#include "tasks.h"
#include "filetasks.h"

class Radio
{
public:
  /* FUNCTIONS */
  Radio();
  void InitTuner();
  void TunerConfigCommands();

  void SwitchSource(Sources source, bool performTune = true);
  void TuneFrequency(uint16_t freq, Sources source);
  void TuneFrequency(uint16_t freq);
  void TuneManual(bool up, bool minStep);
  void TuneSeek(bool up);
  void StopSeek();
  void TuneEnterSrc(Sources source, int8_t freqNum = -1, bool forceTune = false);
  void TuneEnter(int8_t freqNum = -1, bool forceTune = false);
  void SavePreset(uint8_t preset);
  void RecallPreset(uint8_t preset);
  bool UltimateTune(uint8_t setFreqMode, bool resetRds, bool resumeSignalLoop, bool GuiMainClear, bool GuiRdsClear, bool GuiCacheRds, bool GuiBandPreset);
  void SetStereoMode(bool isStereo);
  void SetCurrentVolume(uint8_t volume);
  void SetCurrentVolume(bool up);

  /* TASKS */
  static void LoopSignalStrength(void *params);
  static void LoopRdsDecode(void *params);
  static void Seek(void *params);

  /* VARS */
  Gui *GuiRef;
  Rds rds;
  Station currentStation;
  uint8_t currentPreset;
  uint8_t currentVolume;
  Config config;

private:
  /* FUNCTIONS */
  void ReadLastStation();
  void ClearStation(Sources newSource, uint16_t newFreq);
  // void StopTasks();
  void SetDefaultStation(Station *station, Sources source);
  bool IsValidFreq(uint16_t freq, Sources source);
  int8_t CalcStep(bool minStep, uint16_t freq, Sources source);
  uint16_t AddStepToFrequency(uint16_t freq, uint8_t step, bool up, Sources source);
  void CheckPi();

  /* VARS */
  SeekSates seekState;
  bool tuneInProgress;
  uint16_t enterFreq;
  Sources enterSource;
};

#endif