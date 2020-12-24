#ifndef TEF668X_h
#define TEF668X_h

#include "I2C.h"
#include "TEF668X/boot.h"
#include "tasks.h"

#define MAX_OPSTATUS_TRY    10

/* ENUMS */
typedef enum
{
	TEF665X_MODULE_FM = 32,
	TEF665X_MODULE_AM = 33,
	TEF665X_MODULE_AUDIO   = 48,
	TEF665X_MODULE_APPL  = 64
} TEF668x_MODULE;

typedef enum
{
	TEF665X_Cmd_Tune_To  =  1,
	TEF665X_Cmd_Set_Tune_Options   =2,
	TEF665X_Cmd_Set_Bandwidth      =10,
	TEF665X_Cmd_Set_RFAGC            =11,
	TEF665X_Cmd_Set_Antenna =12,

	TEF665X_Cmd_Set_MphSuppression = 20,
	TEF668X_Cmd_Set_ChannelEqualizer = 22,	//only Lithio avaliable
	TEF665X_Cmd_Set_NoiseBlanker = 23,
	TEF665X_Cmd_Set_NoiseBlanker_Audio = 24,

	TEF665X_Cmd_Set_DigitalRadio = 30,
	TEF665X_Cmd_Set_Deemphasis = 31,
	TEF668X_Cmd_Set_StereoImprovement = 32,	//only Lithio avaliable

	TEF665X_Cmd_Set_LevelStep = 38,
	TEF665X_Cmd_Set_LevelOffset = 39,

	TEF665X_Cmd_Set_Softmute_Time = 40,
	TEF665X_Cmd_Set_Softmute_Mod = 41,
	TEF665X_Cmd_Set_Softmute_Level = 42,
	TEF665X_Cmd_Set_Softmute_Noise = 43,
	TEF665X_Cmd_Set_Softmute_Mph = 44,
	TEF665X_Cmd_Set_Softmute_Max = 45,

	TEF665X_Cmd_Set_Highcut_Time = 50,
	TEF665X_Cmd_Set_Highcut_Mod = 51,
	TEF665X_Cmd_Set_Highcut_Level = 52,
	TEF665X_Cmd_Set_Highcut_Noise = 53,
	TEF665X_Cmd_Set_Highcut_Mph = 54,
	TEF665X_Cmd_Set_Highcut_Max = 55,
	TEF665X_Cmd_Set_Highcut_Min = 56,
	TEF665X_Cmd_Set_Lowcut_Min = 58,
	
	TEF665X_Cmd_Set_Stereo_Time = 60,
	TEF665X_Cmd_Set_Stereo_Mod = 61,
	TEF665X_Cmd_Set_Stereo_Level = 62,
	TEF665X_Cmd_Set_Stereo_Noise = 63,
	TEF665X_Cmd_Set_Stereo_Mph = 64,
	TEF665X_Cmd_Set_Stereo_Max = 65,
	TEF665X_Cmd_Set_Stereo_Min = 66,
	
	TEF668X_Cmd_Set_StHiBlend_Time = 70,	//only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Mod = 71, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Level  = 72, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Noise  = 73, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Mph  = 74, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Max  = 75, //only Lithio avaliable
	TEF668X_Cmd_Set_StHiBlend_Min  = 76, //only Lithio avaliable

	TEF665X_Cmd_Set_Scaler = 80,
	TEF665X_Cmd_Set_RDS = 81,
	TEF665X_Cmd_Set_QualityStatus = 82,
	TEF665X_Cmd_Set_DR_Blend = 83,
	TEF665X_Cmd_Set_DR_Options = 84,
	TEF665X_Cmd_Set_Specials = 85,

	TEF668X_Cmd_Set_StBandBlend_Time = 90,	//only Lithio avaliable
	TEF668X_Cmd_Set_StBandBlend_Gain = 91, //only Lithio avaliable
	TEF668X_Cmd_Set_StBandBlend_Bias  = 92, //only Lithio avaliable
	
	TEF665X_Cmd_Get_Quality_Status = 128,
	TEF665X_Cmd_Get_Quality_Data = 129,
	TEF665X_Cmd_Get_RDS_Status = 130,
	TEF665X_Cmd_Get_RDS_Data = 131,
	TEF665X_Cmd_Get_AGC = 132,
	TEF665X_Cmd_Get_Signal_Status = 133,
	TEF665X_Cmd_Get_Processing_Status = 134,
	TEF665X_Cmd_Get_Interface_Status = 135,
} TEF668x_RADIO_COMMAND;

typedef enum
{
	TEF665X_Cmd_Set_Volume = 10,
	TEF665X_Cmd_Set_Mute = 11,
	TEF665X_Cmd_Set_Input = 12,
	TEF665X_Cmd_Set_Output_Source = 13,
	
	TEF665X_Cmd_Set_Ana_Out = 21,
	TEF665X_Cmd_Set_Dig_IO = 22,
	TEF665X_Cmd_Set_Input_Scaler = 23,
	TEF665X_Cmd_Set_WaveGen = 24
} TEF668x_AUDIO_COMMAND;

typedef enum
{
	TEF665X_Cmd_Set_OperationMode = 1,
	TEF665X_Cmd_Set_GPIO = 3,
	TEF665X_Cmd_Set_ReferenceClock = 4,
	TEF665X_Cmd_Activate = 5,

	TEF665X_Cmd_Get_Operation_Status = 128,
	TEF665X_Cmd_Get_GPIO_Status = 129,
	TEF665X_Cmd_Get_Identification = 130,
	TEF665X_Cmd_Get_LastWrite = 131
} TEF668x_APPL_COMMAND;


/* STRUCTS*/
struct QualityData {
    bool Status;
    bool Af;
    int8_t Level;
    uint8_t Bandwidth;
    uint8_t Modulation;
    uint8_t Usn;
};
struct RdsStatus {
    bool IsDataAvailable;
    bool IsDataLoss;
    bool IsFirstPi;
    bool IsGroupB;
    bool IsSync;
};
struct RdsData {
    bool DataAvailable;
    uint16_t BlockA;
    uint16_t BlockB;
    uint16_t BlockC;
    uint16_t BlockD;
	uint8_t ErrorA;
	uint8_t ErrorB;
	uint8_t ErrorC;
	uint8_t ErrorD;
};

/* FUNCTIONS */
bool Boot();
bool SwitchOnOff(bool on);
/* Set commands */
bool SetFrequency(uint16_t freq, bool fm, uint8_t mode);
bool SetStereoMin(uint8_t mode, uint16_t limit);
/* Get commands */
uint8_t GetOperationStatus(uint8_t *status);
bool GetQuality(bool fm, QualityData *result);
bool GetRdsStatus(RdsStatus *result);
bool GetRdsData(RdsData *result);

/* I2C wrapper - SEMAPHORE*/
int TunerSetCmd(TEF668x_MODULE module, uint8_t cmd, int len,...);
int TunerGetCmd(TEF668x_MODULE module, uint8_t cmd, uint8_t *receive,int len);

#endif
