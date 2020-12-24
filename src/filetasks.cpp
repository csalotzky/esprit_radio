#include "filetasks.h"

/* LAST SOURCE */

bool SaveLastSourceSPIFFS(Sources source) {
    // Load file
    File file = SPIFFS.open("/lastsource", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("ERROR | There was an error opening the file for writing");
        return false;
    }

    // Create buffer
    char data[4]; 
    sprintf(data, "%d", source); 
        
    // Write to file
    if (file.print(data)) {
        Serial.println("File was written");
        Serial.println(data);
        file.close();
        return true;
    }
    else {
        Serial.println("File write failed");
        file.close(); 
        return false;
    }
}

bool ReadLastSourceSPIFFS(Sources *source) {
    // Check if file exists
    if (!SPIFFS.exists("/lastsource")) {
        Serial.println("File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/lastsource", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char *buffer = (char*) malloc (file.size());
    file.readBytes(buffer,file.size());
    *source = static_cast<Sources>(atoi(buffer));
    Serial.print("Parsed source: ");
    Serial.println(*source);
 
    file.close();
    return true;
}

/* LAST STATION */

bool SaveSourceLastStationSPIFFS(uint8_t preset, Station station) {
    // Save last source
    SaveLastSourceSPIFFS(station.Source);

    // Load file
    File file = SPIFFS.open(String("/") + sourceLUT[station.Source], FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println(F("ERROR | There was an error opening the file for writing"));
        //xSemaphoreGive(spiSemaphore);
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1]; 
    sprintf(data, "%d,%d,%d,%d,%s,%s,%s,", preset, station.Frequency, station.RdsPi, station.RdsEcc, station.RdsPs, station.WebName, station.WebUuid); 
    
    // Write to file
    if (file.print(data)) {
        Serial.println("File was written");
        file.close();
        return true;
    }
    else {
        Serial.println("File write failed");
        file.close(); 
        return false;
    }
}

bool ReadSourceLastStationSPIFFS(Sources source, uint8_t *preset, Station *station) {
    // Check if file exists
    if (!SPIFFS.exists(String("/") + sourceLUT[source])) {
        Serial.println("File was not found");
        //xSemaphoreGive(spiSemaphore);
        return false;
    }

    // Load file
    File file = SPIFFS.open(String("/") + sourceLUT[source], FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
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

    // Parse from char array - NOT WORKING
    //sscanf(buffer,"%d,%d,%d,%d,%s,%s,%s,", preset, &station->Frequency, &station->RdsPi, &station->RdsEcc, &station->RdsPs, &station->WebName, &station->WebUuid);
       
    file.close();
    return true;
}

bool SaveConfigSPIFFS(Config config) {
    // Load file
    File file = SPIFFS.open("/config", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("ERROR | There was an error opening the file for writing");
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
    for (size_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        char keyBuffer[10];
        sprintf(keyBuffer,"%d,%d,%d;",config.KeyMaps[i].Key, config.KeyMaps[i].LongPress, config.KeyMaps[i].CmdIndex);
        strcat(buffer,keyBuffer);
    }

    Serial.println(buffer);
    
    // Write to file
    if (file.print(buffer)) {
        Serial.println("File was written");
        file.close();
        return true;
    }
    else {
        Serial.println("File write failed");
        file.close(); 
        return false;
    }
}

bool ReadConfigSPIFFS(Config *config) {
    // Check if file exists
    if (!SPIFFS.exists("/config")) {
        Serial.println("File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/config", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
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

    file.close();
    return true;
}

/* PRESET */
bool SavePresetSPIFFS(Station station, uint8_t preset) {
    // Get current presets
    Station presets[TOTAL_PRESETS];
    ReadAllPresetsSPIFFS(presets);
    presets[preset-1] = station;
    
    // Load file
    File file = SPIFFS.open("/presets", FILE_WRITE);

    // File open error
    if (!file) {
        Serial.println("ERROR | There was an error opening the file for writing");
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

bool ReadPresetByNumberSPIFFS(Station *station, uint8_t preset) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
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
    
    file.close();
    return station->Frequency;
}

bool ReadPresetByPiSPIFFS(Station *station, uint8_t *preset, uint16_t pi) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
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
        Serial.println(*preset);

    }
    
    file.close();
    return station->RdsPi == pi;
}

bool ReadAllPresetsSPIFFS(Station stations[]) {
    // Check if file exists
    if (!SPIFFS.exists("/presets")) {
        Serial.println("File was not found");
        return false;
    }

    // Load file
    File file = SPIFFS.open("/presets", FILE_READ);

    // File open error    
    if (!file) {
        Serial.println("There was an error opening the file for reading");
        return false;
    }

    // Create buffer
    char data[3+1+5+1+4+1+2+1+8+1+32+1+32+1];
    uint8_t i=0;
    Serial.println("---/presets content---");
    while (file.available() && i<TOTAL_PRESETS) {
        int l = file.readBytesUntil('\n', data, sizeof(data));
        data[l] = 0;
                Serial.println(data);

        ParseStation(data, &stations[i]);
        //Serial.println(stations[i].Frequency);
        i++;    
    }
    Serial.println("---/presets end---");


    file.close();
    return true;
}

/* HELPERS */

void ParseKeyMaps(char *input, KeyMap keymap[], uint8_t mapLength) {
    uint8_t i=0;
    char *p = input;
    while (i<mapLength && p[0]!='\0')
    {
        keymap[i].Key = atoi(strsep(&p, ","));
        keymap[i].LongPress = atoi(strsep(&p, ","));
        keymap[i].CmdIndex = atoi(strsep(&p, ";"));
        i++;
    }
}

void ParseStation(char *input, Station *station) {
    // Parse from char array
    //char *strsep(char **sp, const char *sep);
    char *p = input;

    station->Source = static_cast<Sources>(atoi(strsep(&p, ",")));
    station->Frequency = atoi(strsep(&p, ","));
    station->RdsPi = atoi(strsep(&p, ","));
    station->RdsEcc = atoi(strsep(&p, ","));
    strcpy(station->RdsPs, strsep(&p, ","));
    strcpy(station->WebName, strsep(&p, ","));
    strcpy(station->WebUuid, p);
}