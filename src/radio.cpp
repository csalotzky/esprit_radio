#include "radio.h"

// Task handles
TaskHandle_t LoopSignalStrengthHandle;
TaskHandle_t LoopRdsDecodeHandle;
TaskHandle_t LoopAfHandle;
TaskHandle_t LoopSearchHandle;
TaskHandle_t SeekHandle;

// Ctor
Radio::Radio() {}

void Radio::InitTuner()
{
    /* Init tuner via i2c */
    Boot();
    // Read config to memory
    // Debug: uncomment in case of config file was fucked up (resets config to default)
    // SaveConfigSPIFFS(config);
    ReadConfigSPIFFS(&config);

    // Init TEF6686 commands
    TunerConfigCommands();

    /* Init tasks */
    xTaskCreate((TaskFunction_t)Radio::LoopSignalStrength, "SignalCheckTask", 8192, this, 1, &LoopSignalStrengthHandle);
    xTaskCreate((TaskFunction_t)Radio::LoopRdsDecode, "RdsDecodeTask", 8192, this, 1, &LoopRdsDecodeHandle);
    xTaskCreate((TaskFunction_t)Radio::Seek, "SeekTask", 4096, this, 0, &SeekHandle);

    ReadLastStation();
}

// Called on init and when config has changed
void Radio::TunerConfigCommands()
{
    SetStereoMode(config.IsStereo);
    SetCurrentVolume((uint8_t)50);
}

/* STATION HELPERS */

// Ultimate tune method, wraps TEF6686 tune command, handles tasks and gui. (CurrentStation must be up to date, when this method called!!)
bool Radio::UltimateTune(uint8_t setFreqMode, bool resetRds, bool resumeSignalLoop, bool guiMainClear, bool guiRdsClear, bool guiCacheRds, bool guiBandPreset)
{
    tuneInProgress = true;

    /* SEND COMMAND TO TUNER */
    if (currentStation.Source == SOURCE_LW ||
        currentStation.Source == SOURCE_MW ||
        currentStation.Source == SOURCE_SW ||
        currentStation.Source == SOURCE_FM)
        if (!SetFrequency(currentStation.Frequency, currentStation.Source == SOURCE_FM, setFreqMode))
        {
            tuneInProgress = false;
            return false;
        }

    /* HANDLE TASKS */
    if (resetRds)
    {
        rds.Reset();
    }
    if (currentStation.Source == SOURCE_FM)
        vTaskResume(LoopRdsDecodeHandle);
    if (resumeSignalLoop && (currentStation.Source == SOURCE_FM ||
                             currentStation.Source == SOURCE_LW ||
                             currentStation.Source == SOURCE_MW ||
                             currentStation.Source == SOURCE_SW))
        vTaskResume(LoopSignalStrengthHandle);

    /*  GUI */
    if (guiMainClear)
        GuiRef->ClearMain(true);

    if (guiBandPreset)
        GuiRef->DisplaySourcePreset(currentStation.Source, currentPreset);
    if (guiRdsClear)
        GuiRef->ClearRds();
    if (guiCacheRds)
    {
        if (currentStation.RdsPi)
            GuiRef->DisplayRdsPi(currentStation.RdsPi, 0);
        GuiRef->DisplayRdsPs(currentStation.RdsPs, 0);
    }

    GuiRef->DisplayFreq(currentStation.Frequency);

    tuneInProgress = false;
    return true;
}

// Clears CurrentStation, only keeps source and freq, given from parameter
void Radio::ClearStation(Sources newSource, uint16_t newFreq)
{
    currentStation = {};
    currentPreset = 0;
    strcpy(currentStation.RdsPs, "");
    strcpy(currentStation.WebName, "");
    strcpy(currentStation.WebUuid, "");
    currentStation.Source = newSource;
    currentStation.Frequency = newFreq;
}

void Radio::SetCurrentVolume(uint8_t volume)
{
    currentVolume = volume;
    SetVolume(volume);
}

void Radio::SetCurrentVolume(bool up)
{
    int8_t vol = currentVolume + (up ? 5 : -5);
    SetCurrentVolume((uint8_t)(vol < 0 ? 0 : vol > 100 ? 100
                                                       : vol));
}

// Sets band lowest freq (called when last listened station could not retrieved)
void Radio::SetDefaultStation(Station *station, Sources source)
{
    station->Frequency =
        source == SOURCE_FM ? Regions[config.Region].FmMin : source == SOURCE_LW ? Regions[config.Region].LwMin
                                                         : source == SOURCE_MW   ? Regions[config.Region].MwMin
                                                         : source == SOURCE_SW   ? Regions[config.Region].SwMin
                                                                                 : 0;
}

/* TUNE */

// This method called when device has powered up/returned from standby
void Radio::ReadLastStation()
{
    currentStation = {};

    // Get last listened source - fallback: FM
    if (!ReadLastSourceSPIFFS(&currentStation.Source))
        currentStation.Source = SOURCE_FM;
    Serial.println(currentStation.Source);

    // Get last listened station - fallback: Lowest freq of band
    if (!(ReadSourceLastStationSPIFFS(currentStation.Source, &currentPreset, &currentStation) && IsValidFreq(currentStation.Frequency, currentStation.Source)))
        SetDefaultStation(&currentStation, currentStation.Source);

    UltimateTune(1, 0, 1, 0, 0, 1, 1);
}

// Switch between sources, bands. disbling performTune will prevent method from tune to default frequency
void Radio::SwitchSource(Sources source, bool performTune)
{
    // Store prev source state
    if (currentStation.Source == SOURCE_LW ||
        currentStation.Source == SOURCE_MW ||
        currentStation.Source == SOURCE_SW ||
        currentStation.Source == SOURCE_FM ||
        currentStation.Source == SOURCE_WEB)
        SaveSourceLastStationSPIFFS(currentPreset, currentStation);

    // Store new source id
    if (source != SOURCE_STANDBY)
        SaveLastSourceSPIFFS(source);

    if (performTune)
    {
        // Switch on-off tuner
        SwitchOnOff(
            source == SOURCE_LW ||
                    source == SOURCE_MW ||
                    source == SOURCE_SW ||
                    source == SOURCE_FM
                ? 1
                : 0);

        ClearStation(source, 0);

        // TODO - Wait some time, to prevent fals RDS update on new band
        vTaskDelay(100 / portTICK_PERIOD_MS);

        // Parse station - Get last listened station - fallback: Lowest freq of band
        if (!(ReadSourceLastStationSPIFFS(currentStation.Source, &currentPreset, &currentStation) && IsValidFreq(currentStation.Frequency, currentStation.Source)))
            SetDefaultStation(&currentStation, currentStation.Source);

        // Tune!
        UltimateTune(1, 1, 1, 1, 0, 1, 1);
    }
}

// Classic manual tune method
void Radio::TuneManual(bool up, bool minStep)
{
    // Parse freq
    uint16_t freq = AddStepToFrequency(currentStation.Frequency, CalcStep(minStep, currentStation.Frequency, currentStation.Source), up, currentStation.Source);

    // Reset current station
    ClearStation(currentStation.Source, freq);

    // Tune!
    UltimateTune(currentStation.Source == SOURCE_FM ? 4 : 1, 1, 0, 0, 1, 0, 1);
}

// Classic seek method
void Radio::TuneSeek(bool up)
{
    seekState = up ? SEEK_UP : SEEK_DOWN;
    ClearStation(currentStation.Source, currentStation.Frequency);
    vTaskResume(SeekHandle);
}

void Radio::StopSeek()
{
    seekState = SEEK_STOP;
}

// Tune to given freq - Used in enter frequency mode
void Radio::TuneFrequency(uint16_t freq)
{
    ClearStation(currentStation.Source, freq);
    UltimateTune(1, 1, 0, 0, 1, 0, 1);
}

// Tune to given freq and switch band - Used in enter frequency mode
void Radio::TuneFrequency(uint16_t freq, Sources source)
{
    SwitchSource(source, false);
    ClearStation(source, freq);
    UltimateTune(1, 1, 1, 1, 0, 0, 1);
}

/* PRESET FUNCTIONS */

// Saves current station to given preset (presets indexed from 1!)
void Radio::SavePreset(uint8_t preset)
{
    Serial.println("Save preset called!");
    if (SavePresetSPIFFS(currentStation, preset))
    {
        // Success!
        currentPreset = preset;
        char tmp[100];
        sprintf(tmp, "%s #%d", sourceLUT[currentStation.Source], preset);
        GuiRef->DisplaySourcePreset(currentStation.Source, currentPreset);
        GuiRef->DisplayMsgBox(tmp, MSG_PRESETSAVED, 3);
    }
}

// Recalls preset
void Radio::RecallPreset(uint8_t preset)
{
    Serial.println("Recall preset called!");
    Station buff;
    if (preset && preset <= TOTAL_PRESETS && ReadPresetByNumberSPIFFS(&buff, preset))
    {
        currentPreset = preset;
        if (buff.Source != currentStation.Source)
        {
            SwitchSource(buff.Source, false);
            currentStation = buff;
            UltimateTune(1, 1, 1, 1, 0, 1, 1);
        }
        else
        {
            currentStation = buff;
            UltimateTune(1, 1, 0, 0, 1, 1, 1);
        }
    }
    else
    {
        GuiRef->DisplayMsgBox(ERROR_HEADER, ERROR_INVALIDPRESET, 3);
    }
}

/* TUNER ENTER FUNCTIONS */

// -1: Exit and null enterfreq
// First 0: Enter TuneEnter mode without initial num
// 1...9 valid digits
// ForceTune==1: Try to tune enterFreq (basically acts as enter button with extra steps) (enabling this will ignores freqNum)
void Radio::TuneEnter(int8_t freqNum, bool forceTune)
{
    TuneEnterSrc(enterSource == SOURCE_STANDBY ? currentStation.Source : enterSource, freqNum, forceTune);
}

// -1: Exit and null enterfreq
// First 0: Enter TuneEnter mode without initial num
// 1...9 valid digits
// ForceTune==1: Try to tune enterFreq (basically acts as enter button with extra steps) (enabling this will ignores freqNum)
void Radio::TuneEnterSrc(Sources source, int8_t freqNum, bool forceTune)
{
    enterSource = source;

    /* -1 -> Exit */
    if (freqNum == -1 && !forceTune)
    {
        enterFreq = 0;
        enterSource = SOURCE_STANDBY;
        GuiRef->RestoreLast();
        return;
    }

    /* Parse freq and inner validation processes */
    uint16_t bandMin, bandMax = 0;
    // Calc stuffs if forceTune was not triggered
    if (!forceTune)
    {
        // Add digit to current
        enterFreq *= 10;
        enterFreq += freqNum;

        // enterFreq == 0 -> This method was initialized without initial num -> empty enterfreq dialog
        if (!enterFreq)
        {
            GuiRef->DisplayEnterFreq(0, source);
            return;
        }

        // INVALID CHECK No.1 - Check first digit (if its first) and validate
        /*
            (DX)
            FM: min: 6, max: 1 -> 1,6,7,8,9
            LW: min: 1, max: 2 -> 1,2
            MW: min: 5, max: 1 -> 1,5,6,7,8,9
            SW: min: 2: max: 2 -> 2
        */
        bandMin =
            source == SOURCE_FM ? Regions[config.Region].FmMin : source == SOURCE_LW ? Regions[config.Region].LwMin
                                                             : source == SOURCE_MW   ? Regions[config.Region].MwMin
                                                                                     : Regions[config.Region].SwMin;
        bandMax =
            source == SOURCE_FM ? Regions[config.Region].FmMax : source == SOURCE_LW ? Regions[config.Region].LwMax
                                                             : source == SOURCE_MW   ? Regions[config.Region].MwMax
                                                                                     : Regions[config.Region].SwMax;

        if (enterFreq < 10)
        {
            uint16_t minFirstDigit = bandMin;
            uint16_t maxFirstDigit = bandMax;
            while (minFirstDigit >= 10)
                minFirstDigit /= 10;
            while (maxFirstDigit >= 10)
                maxFirstDigit /= 10;
            // bool minIsLower = minFirstDigit < maxFirstDigit;

            if (!(enterFreq >= minFirstDigit || enterFreq <= maxFirstDigit))

                if (enterFreq < minFirstDigit && enterFreq > maxFirstDigit)
                {
                    // INVALID FREQ -> error msg
                    enterFreq = 0;
                    GuiRef->DisplayMsgBox(ERROR_HEADER, ERROR_INVALIDFREQ, 3);
                    return;
                }
        }

        // INVALID CHECK No.2 - Is freq greater than band's max freq
        if ((source == SOURCE_FM ? enterFreq * 10 : enterFreq) > bandMax)
        {
            // INVALID FREQ -> error msg
            enterFreq = 0;
            enterSource = SOURCE_STANDBY;
            GuiRef->DisplayMsgBox(ERROR_HEADER, ERROR_INVALIDFREQ, 3);
            return;
        }
    }

    /* Try to parse valid freq */
    if ((source == SOURCE_FM ? enterFreq * 10 : enterFreq) * 10 > bandMax || forceTune)
    {
        if (IsValidFreq((source == SOURCE_FM ? enterFreq * 10 : enterFreq), source))
        {
            GuiRef->RestoreLast();
            // Serial.println(source == SOURCE_FM ? enterFreq*10 : enterFreq);
            if (currentStation.Source == source)
                TuneFrequency(source == SOURCE_FM ? enterFreq * 10 : enterFreq);
            else
                TuneFrequency(source == SOURCE_FM ? enterFreq * 10 : enterFreq, source);
            enterFreq = 0;
            enterSource = SOURCE_STANDBY;
            return;
        }
        else
        {
            // INVALID FREQ -> error msg
            enterFreq = 0;
            enterSource = SOURCE_STANDBY;
            GuiRef->DisplayMsgBox(ERROR_HEADER, ERROR_INVALIDFREQ, 3);
            return;
        }
    }

    // None of above continue with display digit
    GuiRef->DisplayEnterFreq(enterFreq, source);
}

/* CHECK PI */

// Checks if currentstation's pi can be found in preset list or PS cache - SHOULD ONLY CALL IF PI IS VALID (>0)
// TODO: This should be a low priority task
void Radio::CheckPi()
{
    Station buff;
    uint8_t preset = 0;
    if (ReadPresetByPiSPIFFS(&buff, &preset, currentStation.RdsPi))
    {
        currentPreset = preset;
        strcpy(currentStation.RdsPs, buff.RdsPs);

        GuiRef->DisplayRdsPs(currentStation.RdsPs, 0);
        GuiRef->DisplaySourcePreset(currentStation.Source, currentPreset);
    }
}

/* TUNE HELP FUNCTIONS */
int8_t Radio::CalcStep(bool minStep, uint16_t freq, Sources source)
{
    if (source == SOURCE_FM)
        return minStep ? Regions[config.Region].FmMinStep : Regions[config.Region].FmMaxStep;
    if (source == SOURCE_LW)
        return minStep ? Regions[config.Region].LwMinStep : Regions[config.Region].LwMaxStep;
    if (source == SOURCE_MW)
        return minStep ? Regions[config.Region].MwMinStep : Regions[config.Region].MwMaxStep;
    if (source == SOURCE_SW)
        return minStep ? Regions[config.Region].SwMinStep : Regions[config.Region].SwMaxStep;
    return -1;
}

uint16_t Radio::AddStepToFrequency(uint16_t freq, uint8_t step, bool up, Sources source)
{
    // Calc new frequency
    uint16_t tempFreq;
    if (up)
        tempFreq = (freq + step) / step * step;
    else
        tempFreq = freq % step != 0 ? freq / step * step : freq - step;

    // Check if valid and correct
    if (source == SOURCE_FM)
    {
        if (tempFreq < Regions[config.Region].FmMin)
            tempFreq = Regions[config.Region].FmMax;
        else if (tempFreq > Regions[config.Region].FmMax)
            tempFreq = Regions[config.Region].FmMin;
    }
    if (source == SOURCE_LW)
    {
        if (tempFreq < Regions[config.Region].LwMin)
            tempFreq = Regions[config.Region].LwMax;
        else if (tempFreq > Regions[config.Region].LwMax)
            tempFreq = Regions[config.Region].LwMin;
    }
    else if (source == SOURCE_MW)
    {
        if (tempFreq < Regions[config.Region].MwMin)
            tempFreq = Regions[config.Region].MwMax;
        else if (tempFreq > Regions[config.Region].MwMax)
            tempFreq = Regions[config.Region].MwMin;
    }
    else if (source == SOURCE_SW)
    {
        if (tempFreq < Regions[config.Region].SwMin)
            tempFreq = Regions[config.Region].SwMax;
        else if (tempFreq > Regions[config.Region].SwMax)
            tempFreq = Regions[config.Region].SwMin;
    }

    return tempFreq;
}

// Decides whether given frequency is within band's freq range
bool Radio::IsValidFreq(uint16_t freq, Sources source)
{
    if (source == SOURCE_FM)
        return freq >= Regions[config.Region].FmMin && freq <= Regions[config.Region].FmMax;
    if (source == SOURCE_LW)
        return freq >= Regions[config.Region].LwMin && freq <= Regions[config.Region].LwMax;
    if (source == SOURCE_MW)
        return freq >= Regions[config.Region].MwMin && freq <= Regions[config.Region].MwMax;
    if (source == SOURCE_SW)
        return freq >= Regions[config.Region].SwMin && freq <= Regions[config.Region].SwMax;
    return false;
}

/* OTHER FUNCTIONS */
void Radio::SetStereoMode(bool isStereo)
{
    SetStereoMin(isStereo ? 0 : 2, 400);
}

/* TASKS */

// TODO: check status bit
// Repeatedly reads tuner signal
void Radio::LoopSignalStrength(void *params)
{
    while (true)
    {
        Radio *radioRef = (Radio *)params;
        while (radioRef->currentStation.Source == SOURCE_FM ||
               radioRef->currentStation.Source == SOURCE_LW ||
               radioRef->currentStation.Source == SOURCE_MW ||
               radioRef->currentStation.Source == SOURCE_SW)
        {
            QualityData qd;
            if (!radioRef->tuneInProgress && GetQuality(radioRef->currentStation.Source == SOURCE_FM, &qd) && qd.Status)
            {
                radioRef->GuiRef->DisplaySignal(qd.Level, SIGNAL_MIN, SIGNAL_MAX); // real range is -20 ... 120 dBuV
                /*Serial.print("STATUS: ");
                Serial.println(qd.Status);
                Serial.print("AF DATA: ");
                Serial.println(qd.Af);
                Serial.print("LEVEL: ");
                Serial.println(qd.Level);
                Serial.print("USN: ");
                Serial.println(qd.Usn);*/
                /*Serial.print("BANDWIDTH: ");
                Serial.println(qd.Bandwidth);
                Serial.print("MODULATION: ");
                Serial.println(qd.Modulation);*/
            }
            else
            {
                Serial.println("TUNER | Error while fetching quality data");
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        vTaskSuspend(NULL);
    }
}

// Repeatedly reads RDS
void Radio::LoopRdsDecode(void *params)
{
    while (true)
    {

        Radio *radioRef = (Radio *)params;
        while (radioRef->currentStation.Source == SOURCE_FM)
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            RdsStatus status;
            if (!radioRef->tuneInProgress && GetRdsStatus(&status))
            {
                /*Serial.print("Is data Avalable: ");
                Serial.println(status.IsDataAvailable);
                Serial.print("Is data Loss: ");
                Serial.println(status.IsDataLoss);
                Serial.print("is first Data Block: ");
                Serial.println(status.IsFirstPi);
                Serial.print("Is group B: ");
                Serial.println(status.IsGroupB);
                Serial.print("Is RDS sync: ");
                Serial.println(status.IsSync);*/
                ((Radio *)params)->GuiRef->DisplayRdsFlag(status.IsSync);
                if (status.IsDataAvailable)
                {
                    // Yay! RDS data is available
                    RdsData data;
                    if (GetRdsData(&data))
                    {
                        radioRef->rds.ReceiveData(data);
                        uint16_t piTemp;
                        uint8_t eccTemp;
                        char psTemp[9], rtTemp[65];
                        bool piSafeTemp, psSafeTemp;
                        if (radioRef->rds.ReadPi(&piTemp, &piSafeTemp))
                        {
                            // Store only first safe PI... and check PI in presets + cache
                            if (piSafeTemp && !radioRef->currentStation.RdsPi)
                            {
                                radioRef->currentStation.RdsPi = piTemp;
                                radioRef->CheckPi();
                            }
                            // Display if: safe || DX mode || No safe pi was sent yet (MIXED mode)
                            if (piSafeTemp || radioRef->config.PiMode == RDS_DX || (radioRef->config.PiMode == RDS_MIXED && !radioRef->currentStation.RdsPi))
                                radioRef->GuiRef->DisplayRdsPi(piTemp, piSafeTemp);
                        }
                        if (radioRef->rds.ReadEcc(&eccTemp))
                        {
                            radioRef->currentStation.RdsEcc = eccTemp;
                            radioRef->GuiRef->DisplayRdsEcc(eccTemp, radioRef->currentStation.RdsPi >> 12);
                        }
                        if (radioRef->rds.ReadPs(psTemp, &psSafeTemp))
                        {
                            // Store only first safe PS
                            if (psSafeTemp && radioRef->currentStation.RdsPs[0] == '\0')
                                strcpy(radioRef->currentStation.RdsPs, psTemp);
                            // Display if: safe || DX mode || No safe ps was sent yet (MIXED mode)
                            if (psSafeTemp || radioRef->config.PsMode == RDS_DX || (radioRef->config.PsMode == RDS_MIXED && radioRef->currentStation.RdsPs[0] == '\0'))
                                radioRef->GuiRef->DisplayRdsPs(psTemp, psSafeTemp);
                        }
                        if (radioRef->rds.ReadRt(rtTemp))
                        {
                            radioRef->GuiRef->DisplayRdsRt(rtTemp);
                        }
                    }
                    else
                        Serial.println("TUNER | Error while fetching RDS DATA");
                }
            }
            else
                Serial.println("TUNER | Error while fetching RDS STATUS");
        }

        vTaskSuspend(NULL);
    }
}

// Seek task
void Radio::Seek(void *params)
{
    while (true)
    {
        Radio *radioRef = (Radio *)params;
        uint16_t tempFreq = radioRef->currentStation.Frequency;
        uint8_t step = radioRef->CalcStep(0, tempFreq, radioRef->currentStation.Source);
        QualityData qd;
        bool found = false;

        while (!found && radioRef->seekState != SEEK_STOP)
        {
            radioRef->currentStation.Frequency = radioRef->AddStepToFrequency(radioRef->currentStation.Frequency, step, radioRef->seekState == SEEK_UP, radioRef->currentStation.Source);
            // Tune to next freq
            radioRef->UltimateTune(2, 1, 0, 0, 1, 0, 1);
            vTaskDelay(SEEK_DELAY / portTICK_PERIOD_MS);
            radioRef->GuiRef->DisplayFreq(radioRef->currentStation.Frequency);

            // Init vars that belongs to this freq
            int16_t sigSum = 0;
            uint8_t usnSum = 0;
            uint8_t i = 0;
            // Check signal
            while (!found && i < SEEK_MAX_ATTEMPT)
            {
                if (GetQuality(radioRef->currentStation.Source == SOURCE_FM, &qd) && qd.Status)
                {
                    i++;
                    // Average sums
                    sigSum += qd.Level;
                    usnSum += qd.Usn;

                    /*Serial.print("attempt:");
                    Serial.println(i);

                    Serial.println(qd.Af);
                    Serial.print("sig: ");
                    Serial.print(qd.Level);
                    Serial.print(" usn: ");
                    Serial.println(qd.Usn);

                    Serial.print("avgsig: ");
                    Serial.print(sigSum/i);
                    Serial.print(" avgusn: ");
                    Serial.println(usnSum/i);*/

                    // If averagesignal is very high or very low (25...60 dBuV) -> Our job is done here,
                    // Very strong signal
                    found = sigSum / i >= 60 || usnSum / i <= 1;

                    // Very poor signal
                    if (sigSum < 25 && usnSum >= 10)
                        break;
                }
                else
                {
                    /*Serial.print("STATUS:");
                    Serial.println(qd.Status);*/

                    // TODO ??? occasionally, when we get here, we need to wait a bit more, otherwise in the next request we will get false results
                    vTaskDelay(30 / portTICK_PERIOD_MS);
                }
                vTaskDelay(SEEK_DELAY / portTICK_PERIOD_MS);
            }

            // Final check (if level is between 25...60)
            if (i == SEEK_MAX_ATTEMPT && !found)
                found = sigSum / (i) >= radioRef->config.SeekLevel && usnSum / (i) <= radioRef->config.SeekUsn;
        }

        // Release mute
        radioRef->seekState = SEEK_STOP;
        SetFrequency(0, radioRef->currentStation.Source == SOURCE_FM, 7);
        vTaskSuspend(NULL);
    }
}