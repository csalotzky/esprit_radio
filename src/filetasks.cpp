#include "filetasks.h"

/* LAST SOURCE */

// Saves current/last source - should be called before switch to another source or power off
bool SaveLastSourceSPIFFS(Sources source) {
    // Load file
    File file = SPIFFS.open("/lastsource", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("SPIFFS | SaveLastSourceSPIFFS - There was an error opening the file for writing");
        return false;
    }

    // Create buffer
    char data[4]; 
    sprintf(data, "%d", source); 
        
    // Write to file
    if (file.print(data)) {
        Serial.println("SPIFFS | SaveLastSourceSPIFFS - File was written");
        file.close();
        return true;
    }
    else {
        Serial.println("SPIFFS | SaveLastSourceSPIFFS - File write failed");
        file.close(); 
        return false;
    }
}

// Reads current/last source
bool ReadLastSourceSPIFFS(Sources *source) {
    // Check if file exists
    if (!SPIFFS.exists("/lastsource")) {
        Serial.println("SPIFFS | ReadLastSourceSPIFFS - File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/lastsource", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadLastSourceSPIFFS - There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char *buffer = (char*) malloc (file.size());
    file.readBytes(buffer,file.size());
    *source = static_cast<Sources>(atoi(buffer));
 
    Serial.println("SPIFFS | ReadLastSourceSPIFFS - File was read successfully");

    file.close();
    return true;
}

/* LAST STATION */

// Saves last listened station of given source - should be called before switch to another source or power off
bool SaveSourceLastStationSPIFFS(uint8_t preset, Station station) {
    // Save last source
    SaveLastSourceSPIFFS(station.Source);

    // Load file
    File file = SPIFFS.open(String("/") + sourceLUT[station.Source], FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("SPIFFS | SaveSourceLastStationSPIFFS - There was an error opening the file for writing");
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1]; 
    sprintf(data, "%d,%d,%d,%d,%s,%s,%s,", preset, station.Frequency, station.RdsPi, station.RdsEcc, station.RdsPs, station.WebName, station.WebUuid); 
    
    // Write to file
    if (file.print(data)) {
        Serial.println("SPIFFS | SaveSourceLastStationSPIFFS - File was written");
        file.close();
        return true;
    }
    else {
        Serial.println("SPIFFS | SaveSourceLastStationSPIFFS - File write failed");
        file.close(); 
        return false;
    }
}

// Reads last listened station of give source
bool ReadSourceLastStationSPIFFS(Sources source, uint8_t *preset, Station *station) {
    // Check if file exists
    if (!SPIFFS.exists(String("/") + sourceLUT[source])) {
        Serial.println("SPIFFS | ReadSourceLastStationSPIFFS - File was not found");
        //xSemaphoreGive(spiSemaphore);
        return false;
    }

    // Load file
    File file = SPIFFS.open(String("/") + sourceLUT[source], FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadSourceLastStationSPIFFS - There was an error opening the file for reading");
        //xSemaphoreGive(spiSemaphore);
        return false;
    }

    // Create buffer
    char *buffer = (char*) malloc (file.size());
    file.readBytes(buffer,file.size());

    // Parse from char array
    char *strsep(char **sp, const char *sep);
    char *p = buffer;

    *preset = (uint8_t)atoi(strsep(&p, ","));
    station->Source = source;
    station->Frequency = atoi(strsep(&p, ","));
    station->RdsPi = atoi(strsep(&p, ","));
    station->RdsEcc = atoi(strsep(&p, ","));
    strcpy(station->RdsPs, strsep(&p, ","));
    strcpy(station->WebName, strsep(&p, ","));
    strcpy(station->WebUuid, strsep(&p, ","));
    
    Serial.println("SPIFFS | ReadSourceLastStationSPIFFS - File was read successfully");

    file.close();
    return true;
}

/* CONFIG */

// Saves config
bool SaveConfigSPIFFS(Config config) {
    // Load file
    File file = SPIFFS.open("/config", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("SPIFFS | SaveConfigSPIFFS - There was an error opening the file for writing");
        return false;
    }

    // Create buffer
    char buffer[512];  
    sprintf(buffer, "STEP=%d\nPSMODE=%d\nPIMODE=%d\nREGION=%d\nRDSAF=%d\nSTEREO=%d\nSEEKLEVEL=%d\nSEEKUSN=%d\nNUMPRESET=%d\nKEYMAPS=", 
        config.IsMinStep, 
        config.PsMode, 
        config.PiMode, 
        config.Region, 
        config.IsRdsAf,
        config.IsStereo,
        config.SeekLevel,
        config.SeekUsn,
        config.IsNumKeyDefaultPreset);
    for (size_t i = 0; i < TOTAL_BUTTONS; i++) {
        char keyBuffer[10];
        sprintf(keyBuffer,"%d,%d,%d;",config.KeyMaps[i].Key, config.KeyMaps[i].LongPress, config.KeyMaps[i].CmdIndex);
        strcat(buffer,keyBuffer);
    }
    
    // Write to file
    if (file.print(buffer)) {
        Serial.println("SPIFFS | SaveConfigSPIFFS - File was written");
        file.close();
        return true;
    }
    else {
        Serial.println("SPIFFS | SaveConfigSPIFFS - File write failed");
        file.close(); 
        return false;
    }
}

// Reads config
bool ReadConfigSPIFFS(Config *config) {
    // Check if file exists
    if (!SPIFFS.exists("/config")) {
        Serial.println("SPIFFS | ReadConfigSPIFFS - File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/config", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadConfigSPIFFS - There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char lineBuffer[TOTAL_BUTTONS*9+11];
    char paramBuffer[10];
    char valueBuffer[TOTAL_BUTTONS*9+1];
    while (file.available()) {
        int l = file.readBytesUntil('\n', lineBuffer, sizeof(lineBuffer));
        lineBuffer[l] = 0;
        sscanf(lineBuffer, "%[^= ] = %s", paramBuffer, valueBuffer);
        
        if (!strcmp(paramBuffer, "STEP")) config->IsMinStep = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "PSMODE")) config->PsMode = static_cast<RdsModes>(atoi(valueBuffer));
        else if (!strcmp(paramBuffer, "PIMODE")) config->PiMode = static_cast<RdsModes>(atoi(valueBuffer));
        else if (!strcmp(paramBuffer, "REGION")) config->Region = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "RDSAF")) config->IsRdsAf = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "STEREO")) config->IsStereo = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "SEEKLEVEL")) config->SeekLevel = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "SEEKUSN")) config->SeekUsn = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "NUMPRESET")) config->IsNumKeyDefaultPreset = atoi(valueBuffer);
        else if (!strcmp(paramBuffer, "KEYMAPS")) ParseKeyMaps(valueBuffer, config->KeyMaps, TOTAL_BUTTONS);
    }

    Serial.println("SPIFFS | ReadConfigSPIFFS - File was read successfully");

    file.close();
    return true;
}

/* PRESET */

// Save given station to given preset
bool SavePresetSPIFFS(Station station, uint8_t preset) {
    // Get current presets
    Station presets[TOTAL_PRESETS];
    ReadAllPresetsSPIFFS(presets);
    presets[preset-1] = station;
    
    // Load file
    File file = SPIFFS.open("/presets", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("SPIFFS | SavePresetSPIFFS - There was an error opening the file for writing");
        return false;
    }

    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1]; 
    for (size_t i = 0; i < TOTAL_PRESETS; i++)
    {
        sprintf(data, "%d,%d,%d,%d,%s,%s,%s\n", presets[i].Source, presets[i].Frequency, presets[i].RdsPi, presets[i].RdsEcc, presets[i].RdsPs, presets[i].WebName, presets[i].WebUuid);
        file.print(data);
    }
    file.close();
    return true;
}

// Reads preset by preset number (indexed from 1!)
bool ReadPresetByNumberSPIFFS(Station *station, uint8_t preset) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("SPIFFS | ReadPresetByNumberSPIFFS - File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadPresetByNumberSPIFFS - There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1];
    uint8_t i=0, l=0;
    while (file.available() && i<preset) {
        l = file.readBytesUntil('\n', data, sizeof(data));
        i++;    
    }
    data[l] = 0;
    ParseStation(data, station);
    
    Serial.println("SPIFFS | ReadPresetByNumberSPIFFS - File was read successfully");

    file.close();
    return station->Frequency;
}

// Search preset by RDS PI
bool ReadPresetByPiSPIFFS(Station *station, uint8_t *preset, uint16_t pi) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("SPIFFS | ReadPresetByPiSPIFFS - File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadPresetByPiSPIFFS - There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1];
    uint8_t l;
    while (file.available() && station->RdsPi!=pi) {
        l = file.readBytesUntil('\n', data, sizeof(data));
        data[l] = 0;
        ParseStation(data, station);
        (*preset)++;
    }
    
    Serial.println("SPIFFS | ReadPresetByPiSPIFFS - File was read successfully");

    file.close();
    return station->RdsPi == pi;
}

// Gets all presets
bool ReadAllPresetsSPIFFS(Station stations[]) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("SPIFFS | ReadAllPresetsSPIFFS - File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("SPIFFS | ReadAllPresetsSPIFFS - There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1];
    uint8_t i=0;
    while (file.available() && i<TOTAL_PRESETS) {
        int l = file.readBytesUntil('\n', data, sizeof(data));
        data[l] = 0;
        ParseStation(data, &stations[i]);
        i++;    
    }

    Serial.println("SPIFFS | ReadAllPresetsSPIFFS - File was read successfully");

    file.close();
    return true;
}

/* HELPERS */

void ParseKeyMaps(char *input, KeyMap keymap[], uint8_t mapLength) {
    uint8_t i=0;
    char *p = input;
    while (i<mapLength && p[0]!='\0') {
        keymap[i].Key = atoi(strsep(&p, ","));
        keymap[i].LongPress = atoi(strsep(&p, ","));
        keymap[i].CmdIndex = atoi(strsep(&p, ";"));
        i++;
    }
}

void ParseStation(char *input, Station *station) {
    char *p = input;

    station->Source = static_cast<Sources>(atoi(strsep(&p, ",")));
    station->Frequency = atoi(strsep(&p, ","));
    station->RdsPi = atoi(strsep(&p, ","));
    station->RdsEcc = atoi(strsep(&p, ","));
    strcpy(station->RdsPs, strsep(&p, ","));
    strcpy(station->WebName, strsep(&p, ","));
    strcpy(station->WebUuid, p);
}