#ifndef FileTasks_h
#define FileTasks_h

#include "FS.h"
#include "SPIFFS.h"
#include "Arduino.h"
#include "definitions.h"

bool SaveLastSourceSPIFFS(Sources source);
bool ReadLastSourceSPIFFS(Sources *source);
bool SaveSourceLastStationSPIFFS(uint8_t preset, Station station);
bool ReadSourceLastStationSPIFFS(Sources source, uint8_t *preset, Station *station);
bool SaveConfigSPIFFS(Config config);
bool ReadConfigSPIFFS(Config *config);
bool SavePresetSPIFFS(Station station, uint8_t preset);
bool ReadPresetByNumberSPIFFS(Station *station, uint8_t preset);
bool ReadPresetByPiSPIFFS(Station *station, uint8_t *preset, uint16_t pi);
bool ReadAllPresetsSPIFFS(Station station[]);

void ParseKeyMaps(char *input, KeyMap *keymap, uint8_t mapLength);
void ParseStation(char *input, Station *station);

#endif