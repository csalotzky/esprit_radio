#include "TEF668X.h"

#define High_16bto8b(a)	((uint8_t)((a) >> 8))
#define Low_16bto8b(a) 	((uint8_t)(a )) 
#define Convert8bto16b(a)	((uint16_t)(((uint16_t)(*(a))) << 8 |((uint16_t)(*(a+1)))))
#define TAB_NUM(tab)	(sizeof(tab)/sizeof(tab[0]))

#define SEMAPHORE_MAX_ATTEMPT   50

// Tuner setup function, returns true if tuner has started
bool Boot() {
    uint8_t status, i=0;
    InitI2C();
    while (!GetOperationStatus(&status) && i < MAX_OPSTATUS_TRY)
    {
        Serial.println("BOOT | WARNING: Unable to connect to tuner!");
        i++;
        delay(1000);
    }

    if (i==MAX_OPSTATUS_TRY) {
        Serial.println("BOOT | ERROR: Unable to connect to tuner!");
        return false;
    }  
    
    if (status > 1) {
        Serial.println("BOOT | WARNING: Tuner is already in active state!");
        return true;
    }
    
    Serial.println("BOOT | INFO: Boot process has started");
    Tuner_Init();
    
    // Final check
    GetOperationStatus(&status);
    return status;
}


// Aka. set operation mode
bool SwitchOnOff(bool on) {
	return TunerSetCmd(TEF665X_MODULE_APPL,
        TEF665X_Cmd_Set_OperationMode, 
        5,
        on ? 0 : 1);
}

/* 
Mode
1 = Preset Tune to new program with short mute time Enable radio and FM/AM change where applicable
2 = Search Tune to new program and stay muted Enable radio and FM/AM change where applicable FM
3 = AF-Update Tune to alternative frequency, store quality and tune back with inaudible mute
4 = Jump Tune to alternative frequency with inaudible mute
5 = Check Tune to alternative 
*/
bool SetFrequency(uint16_t freq, bool fm, uint8_t mode) {
    //Radio_SetFreq(Radio_PRESETMODE, FM1_BAND, freq);
	return TunerSetCmd(fm ? TEF665X_MODULE_FM: TEF665X_MODULE_AM,
			TEF665X_Cmd_Tune_To, 
			(mode<=5)? 7 : 5,
			mode, freq);
}

/* 
Mode
	strong signal handling
	0 = off; channel separation is not limited (default)
	1 = on; minimum control limit set by limit parameter
	2 = forced mono
Limit
	[ 15:0 ]
	Stereo fixed attenuation limit
	60 � 400 [0.1* dB] = 6 � 40 dB Stereo minimum channel separation
	400 = 40 dB (default)
*/
bool SetStereoMin(uint8_t mode, uint16_t limit) {
    return TunerSetCmd(TEF665X_MODULE_FM,
	    TEF665X_Cmd_Set_Stereo_Min,
		7,
		mode, limit);
}

/*
Device operation status
	0 = boot state; no command support
	1 = idle state
	2 = active state; radio standby
	3 = active state; FM
	4 = active state; AM
*/
uint8_t GetOperationStatus(uint8_t *status) {
	uint8_t buf[2];
	int r;
	
	r = TunerGetCmd(TEF665X_MODULE_APPL,
			TEF665X_Cmd_Get_Operation_Status,
			buf,sizeof(buf));

	if(r == 1){
		*status = Convert8bto16b(buf);
	}

	return r;
}

// Get quality status
bool GetQuality(bool fm, QualityData *result) {
	uint8_t buf[14];
	int r;
	int16_t temp;
	
	r = TunerGetCmd(fm ? TEF665X_MODULE_FM : TEF665X_MODULE_AM,
			TEF665X_Cmd_Get_Quality_Data,
			buf,sizeof(buf));

	if(r == 1)
	{
        temp = Convert8bto16b(buf);
        result->Af = temp >> 15;
		result->Status = temp & 0b1111111111; 
		result->Level = (((int16_t)Convert8bto16b(buf+2))/10);
		result->Usn = (Convert8bto16b(buf+4)/10);
		//*wam = (Convert8bto16b(buf+6)/10);
		//temp = (((int16_t)Convert8bto16b(buf+8))/1);
		//*offset = temp<0? (0-temp) : temp;
		result->Bandwidth = (Convert8bto16b(buf+10)/10);
		result->Modulation = (Convert8bto16b(buf+12)/10);
	}

	return r == 1;
}

// Get RDS status
bool GetRdsStatus(RdsStatus *result) {
    uint8_t buf[2];
	int r;
	
	r = TunerGetCmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Get_RDS_Status,
			buf,sizeof(buf));

	if(r == 1){
        uint16_t temp = Convert8bto16b(buf);
        result->IsDataAvailable = temp >> 15;
        result->IsDataLoss = (temp & 0b0100000000000000) >> 14;
        result->IsFirstPi = (temp & 0b0010000000000000) >> 13;
        result->IsGroupB = (temp & 0b0001000000000000) >> 12;
        result->IsSync = (temp & 0b0000001000000000) >> 9;
	}

	return r;
}

// Get RDS data 
bool GetRdsData(RdsData *result) {
    uint8_t buf[12];
	int r;
	
	r = TunerGetCmd(TEF665X_MODULE_FM,
			TEF665X_Cmd_Get_RDS_Data,
			buf,sizeof(buf));

	if(r == 1){
        result->DataAvailable = Convert8bto16b(buf) & 0b1000000000000000 >> 15;
		result->BlockA = Convert8bto16b(buf+2);
		result->BlockB = Convert8bto16b(buf+4);
		result->BlockC = Convert8bto16b(buf+6);
		result->BlockD = Convert8bto16b(buf+8);
        uint16_t temp = Convert8bto16b(buf+10);
        result->ErrorA = (temp & 0b1100000000000000) >> 14;
        result->ErrorB = (temp & 0b0011000000000000) >> 12;
        result->ErrorC = (temp & 0b0000110000000000) >> 10;
        result->ErrorD = (temp & 0b0000001100000000) >> 8;
	}

	return r;
}

/* I2C commands wrapper */
// Universal I2C set wrapper
int TunerSetCmd(TEF668x_MODULE module, uint8_t cmd, int len,...)
{
    bool success = false;
    if(i2cSemaphore != NULL ) {
        uint8_t attempt = 0;
        while (xSemaphoreTake(i2cSemaphore, ( TickType_t ) 100 ) != pdTRUE && attempt != SEMAPHORE_MAX_ATTEMPT) attempt++;
        
        if(attempt!=SEMAPHORE_MAX_ATTEMPT) {
            int i;
            uint8_t buf[20];
            uint16_t temp;
                va_list     vArgs;

                va_start(vArgs, len);
                
            buf[0]= module;	//module,		FM/AM/APP
            buf[1]= cmd;	//cmd,		    1,2,10,... 
            buf[2]= 1;	    //index, 		always 1

            //fill buffer with 16bits one by one
            for(i=3;i<len;i++)
            {
                temp = va_arg(vArgs,int);	//the size only int valid for compile
                
                buf[i++]=High_16bto8b(temp);		
                buf[i]=Low_16bto8b(temp);		
            }
            
            va_end(vArgs);

            success = TunerBufferWrite(buf, len) == 1;

            xSemaphoreGive(i2cSemaphore);
        }
        else {
            Serial.println("FREERTOS | Didn't get semaphore!");
        }
    }
    else {
        Serial.println("FREERTOS | Semaphore is null!");
    }

    return success;
}

// Universal I2C get command
int TunerGetCmd(TEF668x_MODULE module, uint8_t cmd, uint8_t *receive,int len)
{
    bool success = false;
    if(i2cSemaphore != NULL ) {
        uint8_t attempt = 0;
        while (xSemaphoreTake(i2cSemaphore, ( TickType_t ) 100 ) != pdTRUE && attempt != SEMAPHORE_MAX_ATTEMPT) attempt++;

        if(attempt!=SEMAPHORE_MAX_ATTEMPT) {
            uint8_t buf[3];

            buf[0]= module;	//module,		FM/AM/APP
            buf[1]= cmd;	//cmd,		    1,2,10,... 
            buf[2]= 1;	    //index, 		always 1

            success = TunerBufferWrite(buf, 3) == 1;

            if (success) success = TunerBufferRead(receive,len) == 1;

            xSemaphoreGive(i2cSemaphore);
        }
        else {
            Serial.println("FREERTOS | Didn't get semaphore!");
        }
    }
    else {
        Serial.println("FREERTOS | Semaphore is null!");
    }

    return success;
}