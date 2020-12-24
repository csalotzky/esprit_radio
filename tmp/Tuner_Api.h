/***********************************************************************//**
 * @file		Hero.h
 * @purpose		This example used to test NXP HERO function
 * @version		0.1
 * @date		1. Oct. 2010
 * @author		NXP BU Automotive Car Entertainment Team & VITEC ELECTRONICS(SHENZHEN) AE Team
 *---------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors and VITEC Electronics assume no responsibility or liability for the
 * use of the software, convey no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors and VITEC Electronics
 * reserve the right to make changes in the software without
 * notification. NXP Semiconductors and VITEC Electronics also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#ifndef __RADIO_FUNC_H
#define __RADIO_FUNC_H

#include "Arduino.h"
#include "Tuner_Drv_Lithio.h"

//#define RADIO_D3_ENABLE
#define RADIO_LITHIO_ENABLE


typedef enum{
	Radio_Hero,
	Radio_Atomic,
	Radio_Atomic2,
	Radio_Lithio,
	Radio_Helio,
	Radio_Dirana3,

	Radio_Last
}eDev_Type;
extern eDev_Type RadioDev;

#define Is_Radio_Dirana3	(RadioDev == Radio_Dirana3)
#define Is_Radio_Atomic2	(RadioDev == Radio_Atomic2)
#define Is_Radio_Lithio		(RadioDev == Radio_Lithio)
#define Is_Radio_Hero		(RadioDev == Radio_Hero)


#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW  0;
#endif

typedef enum{
	RADIO_PRIMARY,
	RADIO_SECONDARY,

	RADIO_ID_LAST
}RADIO_ID;


/* tuner mode */
#define Radio_POWERDOWNMODE 0
#define Radio_PRESETMODE 0x10
#define Radio_SEARCHMODE 0x20
#define Radio_AFUPDATEMODE 0x30
#define Radio_JUMPMODE 0x40
#define Radio_CHECKMODE 0x50
#define Radio_ENDMODE 0x70

#define TUNER_LEVEL_3dB		3
#define TUNER_LEVEL_5dB		5
#define TUNER_LEVEL_10dB	10
#define TUNER_LEVEL_14dB	14
#define TUNER_LEVEL_20dB	20
#define TUNER_LEVEL_25dB	25
#define TUNER_LEVEL_30dB	30
#define TUNER_LEVEL_35dB	35
#define TUNER_LEVEL_40dB	40
#define TUNER_LEVEL_45dB	45
#define TUNER_LEVEL_50dB	50
#define TUNER_LEVEL_55dB	55
#define TUNER_LEVEL_60dB	60

#define TUNER_PERCENT_27		27
#define TUNER_PERCENT_23		23
#define TUNER_PERCENT_20		20

#define TUNER_OFFSET_10KHz		100
#define TUNER_OFFSET_1500Hz	15
#define TUNER_OFFSET_300KHz		300


#define RADIO_FM_LEVEL_AVAILABLE_TIME	4	//5 ms after tuning
#define RADIO_AM_LEVEL_AVAILABLE_TIME	36	//36 ms after tuning
#define RADIO_USN_AVAILABLE_TIME	34	//34ms

#define FM_SCAN_LEVEL			TUNER_LEVEL_25dB	//dB, 0.5dB/step -8dB~99.5dB
#define AM_SCAN_LEVEL			TUNER_LEVEL_35dB	//dB, 0.5dB/step -8dB~99.5dB
#define FM_SCAN_LEVEL_HI		TUNER_LEVEL_45dB	//dB, 0.5dB/step -8dB~99.5dB
#define AM_SCAN_LEVEL_HI		TUNER_LEVEL_55dB	//dB, 0.5dB/step -8dB~99.5dB

#define FM_USN_DISTURBANCE	TUNER_PERCENT_27	//disturbance = 27%
#define FM_WAM_DISTURBANCE	TUNER_PERCENT_23	//disturbance = 23%

#define FM_FREQ_OFFSET			TUNER_OFFSET_10KHz	//offset = 10k, step=100Hz
#define AM_FREQ_OFFSET			TUNER_OFFSET_1500Hz	//offset = 1.5k, step=100Hz

#define STEREO_Separation_Level_H  TUNER_LEVEL_20dB		//Stereo indication by LEVEL
#define STEREO_Separation_Level_L  TUNER_LEVEL_14dB		//Stereo indication by LEVEL

//RDS search condition
#define RDS_STOP_LEVEL		TUNER_LEVEL_20dB
#define RDS_STOP_USN		TUNER_PERCENT_20
#define RDS_STOP_WAM		TUNER_PERCENT_20
#define RDS_STOP_IFCResult	TUNER_OFFSET_300KHz		//30KHz


/*radio band define*/
#define MaxBandNum 9		 
#define FM1_BAND 0
#define FM2_BAND 1
#define FM3_BAND 2
#define MW_BAND 3
#define LW_BAND 4
#define SW_BAND 5
#define WX_BAND 6
#define DAB_3_BAND 7
#define DAB_L_BAND 8

/*one step define
NOTE:FM Freqency uint is 10KHz,AM Freqency uint is 1KHz*/
#define AM_Step_9k 9  //am step 9khz
#define AM_Step_10k 10 //am step 10khz
#define FM_Step_50k 5 //fm step 50khz
#define FM_Step_100k 10 //fm step 10 0khz
#define FM_Step_200k 20 //fm step 200khz

/*tuner scan/seek condition define*/
#define Radio_ScanLevel 		TUNER_LEVEL_25dB  //dB, 0.5dB/step -8dB~99.5dB
#define Radio_FMScanSen ((uint8_t)((Radio_ScanLevel+8)*2))//DX
#define Radio_FMScanSen_Loc ((uint8_t)((Radio_ScanLevel+TUNER_LEVEL_20dB+8)*2))//   Local Att=DX+20
#define Radio_FMScanUNoise 70
#define Radio_FMScanMNoise 60
#define Radio_FMScanOffset 10

#define Radio_FM_Status 0x20
#define Radio_AMScanSen 		((8+TUNER_LEVEL_35dB)*2)//DX
#define Radio_AMScanSen_Loc 	((8+TUNER_LEVEL_35dB+20)*2)  //   Local Att=DX+20
#define Radio_AMScanOffset 		TUNER_OFFSET_1500Hz
#define Radio_AM_Status 0xA0

#define StereoSeparation_Level_H  ((uint8_t)((TUNER_LEVEL_20dB+8)*2))		//Stereo indication by LEVEL
#define StereoSeparation_Level_L  ((uint8_t)((TUNER_LEVEL_14dB+8)*2))		//Stereo indication by LEVEL
/*check station result */
#define NO_STATION   90
#define PRESENT_STATION        100

/*station max*/
#define MaxStationNum 7/*Station count(6)+current statuin(1)1*/



#define F_UP   1
#define F_DOWN  0
#define StationRecordLen (MaxBandNum*MaxStationNum*2)


#define bit0		(1<<0)
#define bit1		(1<<1)
#define bit2		(1<<2)
#define bit3		(1<<3)
#define bit4		(1<<4)
#define bit5		(1<<5)
#define bit6		(1<<6)
#define bit7		(1<<7)
#define bit8		(1<<8)
#define bit9		(1<<9)
#define bit10		(1<<10)
#define bit11		(1<<11)
#define bit12		(1<<12)
#define bit13		(1<<13)
#define bit14		(1<<14)
#define bit15		(1<<15)
#define bit16		(1<<16)
#define bit17		(1<<17)
#define bit18		(1<<18)
#define bit19		(1<<19)
#define bit20		(1<<20)
#define bit21		(1<<21)
#define bit22		(1<<22)
#define bit23		(1<<23)
#define bit24		(1<<24)
#define bit25		(1<<25)
#define bit26		(1<<26)
#define bit27		(1<<27)
#define bit28		(1<<28)
#define bit29		(1<<29)
#define bit30		(1<<30)
#define bit31		(1<<31)

/*are config parameters struct*/
typedef struct{         //area radio parameter 
	uint32_t FM_MaxFreq;			 //fm max freq
	uint32_t FM_MinFreq;                        //fm min freq
	uint32_t AM_MaxFreq;                       // am max freq
	uint32_t AM_MinFreq;                        //am min freq
	uint32_t FM_AutoSeekStep;               // fm step
	uint32_t FM_ManualSeekStep;           
	uint32_t AM_AutoSeekStep;              //am step
	uint32_t AM_ManualSeekStep;
}Radio_AreaConfigDef;

/*area define*/
enum Radio_AreaCode{  
    Radio_CHN=0,
    Radio_EUR,
    Radio_USA,
    Radio_JPN
};

 /*band freq range*/
typedef struct     
{
    uint16_t MinFreq;
    uint16_t MaxFreq;
}FreqBaundDef;
 
/*Station freq*/
typedef struct 
{
    uint16_t Freq[MaxStationNum];   //0-current work freq,or backup freq;
}StationMemType;

typedef enum
{
	eAR_TuningAction_Standby	  = 0, 
	eAR_TuningAction_Preset 	  = 1, /*!< Tune to new program with short mute time */
	eAR_TuningAction_Search 	  = 2, /*!< Tune to new program and stay muted */
	eAR_TuningAction_AF_Update	  = 3, /*!< Tune to alternative frequency, store quality and tune back with inaudible mute */
	eAR_TuningAction_Jump		  = 4, /*!< Tune to alternative frequency with short inaudible mute  */
	eAR_TuningAction_Check		  = 5, /*!< Tune to alternative frequency and stay muted */
	eAR_TuningAction_End		  = 7  /*!< Release the mute of a Search/Check action (frequency is ignored) */
} AR_TuningAction_t, *pAR_TuningAction_t;


/*band function*/
extern void Radio_SetBand(uint8_t Band);
extern void Radio_NextBand(void);
extern uint8_t Radio_IsFMBand(void);
extern uint8_t Radio_GetCurrentBand(void);

/*preset function*/
extern void Radio_SaveCurrentFreq2Preset(uint8_t Station);
extern void Radio_SelectPreset(uint8_t Station);
extern void Radio_ClearCurrentStation(void);
extern uint8_t Radio_GetCurrentStation(void);

/*radio freqency function*/
extern void Radio_SetFreq(uint8_t mode,uint8_t Band,uint16_t Freq);
extern uint32_t Radio_GetFreqStep(uint8_t band);
extern void Radio_ChangeFreqOneStep(uint8_t UpDown );
extern uint16_t Radio_GetCurrentFreq(void);

/*parameters*/
extern void Radio_Para_Init(void);


/*check station */
extern void Radio_CheckStation(void);
extern void Radio_CheckStationInit(void);
extern uint8_t Radio_CheckStationStatus(void);

/*read/save  radio setup parameters from/ to eeprom*/
void Radio_ReadStationRecord(void);
void Radio_StoreCurrentBand(void);
void Radio_StoreAll(void);
void Radio_Save_Station(uint8_t StationNumber ,uint16_t StationFreq);
void Radio_StoreStationRecord(uint8_t Band);
void Radio_SetSeekSenLevel(uint8_t Lev);
void Radio_StoreStation(uint8_t Band,uint8_t Station);
bool Radio_CheckStereo(void);
int Radio_Get_Level(bool fm);
bool Radio_Is_AF_Update_Available (void); // REVIEW!
bool Radio_Is_RDAV_Available (void); 	//REVIEW!
int Radio_Get_RDS_Data(uint32_t*rds_data);

#endif

