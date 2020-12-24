#include "skinTTGO.h"

#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   135

#define HEADER_BG       0x0
#define MAIN_BG         0x0165
#define TXT_MAIN        0xffff
#define TXT_UNSAFE      0x73AE
#define TXT_HEADER      0xffff

#define HEADER_HEIGHT   20

#define BATT_X          4
#define BATT_Y          4
#define BATT_WIDTH      25
#define BATT_HEIGHT     12
#define BATT_SPACING    2

#define PRESET_X        40
#define PRESET_Y        2

#define RDS_X           90
#define RDS_Y           0
#define RDS_WIDTH       60
#define RDS_HEIGHT      16

#define SIG_X           170
#define SIG_Y           2
#define SIG_BARS        5
#define SIG_BARWIDTH    4
#define SIG_BARHEIGHT   15
#define SIG_BARSPACING  2

#define FREQ_X          5
#define FREQ_Y          30
#define DISPLAY_UNIT    0

#define PI_X            165
#define PI_Y            30

#define COUNTRY_X       PI_X
#define COUNTRY_Y       PI_Y+23

#define PS_X            2
#define PS_Y            95

#define RT_X            2
#define RT_Y            60
#define RT_LINES        2

#define MSGBOX_X        10
#define MSGBOX_Y        HEADER_HEIGHT+10
#define MSGBOX_WIDTH    SCREEN_WIDTH-(2*MSGBOX_X)
#define MSGBOX_HEIGHT   SCREEN_HEIGHT-(HEADER_HEIGHT+2*10)

#define MENU_TXT_X      10
#define MENU_TXT_Y      4
#define MENU_CHECK_X    210
#define MENU_CHECK_Y    4
#define MENU_CHECK_SIZE 16

TFT_eSPI tft = TFT_eSPI(SCREEN_HEIGHT, SCREEN_WIDTH); // Invoke custom library

Skin::Skin() {}

void Skin::InitScreen() {
  Serial.println("HERE");
  tft.init();
  tft.setRotation(1);
  tft.setSwapBytes(true);
  tft.fillRect(0,0,SCREEN_WIDTH,HEADER_HEIGHT,HEADER_BG);
  tft.setTextWrap(false,false);
  InitMain();
}

void Skin::InitMain() {
  tft.fillRect(0,HEADER_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT-HEADER_HEIGHT,MAIN_BG);
  tft.drawLine(0,HEADER_HEIGHT,SCREEN_WIDTH,HEADER_HEIGHT,TFT_WHITE);
}

void Skin::DisplayBattery(int8_t currentPercentage, int8_t lastPercentage) {
  // No change -> no action
  if (currentPercentage == lastPercentage) return;
  
  // Changed
  // Calc batt color
  int battColor;
  if (currentPercentage <= 20) battColor = TFT_RED;
  else battColor = TFT_WHITE;
  
  // Battery icon
  tft.drawRect(BATT_X,BATT_Y,BATT_WIDTH,BATT_HEIGHT,battColor);
  tft.fillRect(BATT_X+BATT_WIDTH,BATT_Y+BATT_HEIGHT*0.2,BATT_SPACING+1,BATT_HEIGHT-BATT_HEIGHT*0.2*2,battColor);

  // Clear last charge level
  tft.fillRect(BATT_X+BATT_SPACING,BATT_Y+BATT_SPACING,(BATT_WIDTH-(2*BATT_SPACING)),BATT_HEIGHT-(2*BATT_SPACING),HEADER_BG);

  // Draw current charge level
  tft.fillRect(BATT_X+BATT_SPACING,BATT_Y+BATT_SPACING,(BATT_WIDTH-(2*BATT_SPACING))*currentPercentage*0.01,BATT_HEIGHT-(2*BATT_SPACING),battColor);
}

void Skin::DisplaySourcePreset(const char* currentSource, uint8_t currentPreset, const char* lastSource, uint8_t lastPreset) {
  // No change -> no action
  if (strcmp(currentSource, lastSource) == 0 && currentPreset == lastPreset) return;
  
  // Changed
  tft.setTextSize(1);
  
  // clear prev
  tft.setTextColor(HEADER_BG);
  char oldStr[10];
  if (lastPreset) sprintf(oldStr, "%s #%d", lastSource, lastPreset);
  else sprintf(oldStr, "%s", lastSource);
  tft.drawString(oldStr,PRESET_X,PRESET_Y,2);

  // print new
  tft.setTextColor(TXT_HEADER);
  char newStr[10];
  if (currentPreset) sprintf(newStr, "%s #%d", currentSource, currentPreset);
  else sprintf(newStr, "%s", currentSource);
  tft.drawString(newStr,PRESET_X,PRESET_Y,2);
}

void Skin::DisplayRdsFlag(bool currentRdsFlag, bool lastRdsFlag) {
  // No change -> no action
  if (currentRdsFlag == lastRdsFlag) return;
  
  // Changed
  if (currentRdsFlag) tft.pushImage(RDS_X,RDS_Y,RDS_WIDTH,RDS_HEIGHT,rdsLogo);
  else tft.fillRect(RDS_X,RDS_Y,RDS_WIDTH,RDS_HEIGHT,HEADER_BG);
}

void Skin::DisplaySignal(int8_t currentSignalValue, int8_t signalMin, int8_t signalMax, int8_t lastSignalValue){
  // No change -> no action
  if (currentSignalValue == lastSignalValue) return;
  
  // Changed
  // Signal bars
  double sigRatio = (double)(currentSignalValue-signalMin)/(double)(signalMax-signalMin);
  int sigColor;
  if (sigRatio < 0.4) sigColor = TFT_RED;
  else if (sigRatio < 0.6) sigColor = TFT_YELLOW;
  else if (sigRatio < 0.8) sigColor = TFT_ORANGE;
  else sigColor = TFT_DARKGREEN;
  
  for (int b=0; b < SIG_BARS; b++) {
    int barX = SIG_X + ((b+1)*(SIG_BARWIDTH+SIG_BARSPACING));             //170 176  182  188  194  200
    int barY = SIG_Y + (double)(SIG_BARS-(b+1))/SIG_BARS*SIG_BARHEIGHT;   //18  14.8 11.6  8.4  5.2 2           // 14...17  11...17 8...17    5...17  2...18
    
    if (sigRatio*SIG_BARS >= b) tft.fillRect(barX,barY,SIG_BARWIDTH,ceil((double)SIG_BARHEIGHT*(b+1)/SIG_BARS),sigColor); 
    else {
      tft.fillRect(barX,barY,SIG_BARWIDTH,ceil((double)SIG_BARHEIGHT*(b+1))/SIG_BARS,HEADER_BG);  //0  3.2  6.4  9.6   12.8   16
      tft.drawRect(barX,barY,SIG_BARWIDTH,ceil((double)SIG_BARHEIGHT*(b+1))/SIG_BARS,sigColor);
    }
  }

  // Text - cheat: no enough space to print numbers with more than 2 digits -> narrowing limits to print  0...99
  tft.setTextSize(2);
  tft.setTextColor(HEADER_BG);
  tft.drawNumber(lastSignalValue < 0 ? 0 : lastSignalValue > 99 ? 99 : lastSignalValue, SIG_X+((SIG_BARS+1)*(SIG_BARWIDTH+SIG_BARSPACING))+SIG_BARWIDTH, SIG_Y);
  tft.setTextColor(TXT_HEADER);
  tft.drawNumber(currentSignalValue < 0 ? 0 : currentSignalValue > 99 ? 99 : currentSignalValue, SIG_X+((SIG_BARS+1)*(SIG_BARWIDTH+SIG_BARSPACING))+SIG_BARWIDTH, SIG_Y);
}

void Skin::DisplayFreq(const char* currentFrequency, const char* freqUnit, const char* lastFrequency) {
  /* FREQUENCY */
  tft.setTextSize(4);

  uint8_t i, j; // i: iterating over currentFrequency, j: iterating over lastFrequency
  bool lastEndCharFound = false;
  j = 0;

  // Go through new frequency, check whether previous frequency was shorter and clear only changed characters (less flickering)
  for (i = 0; currentFrequency[i] != '\0' ; i++){
    //check last char -> store if end
    if (!lastEndCharFound && lastFrequency[i] != '\0') j = i;
    else lastEndCharFound = true;

    if (currentFrequency[i] != lastFrequency[i] || lastEndCharFound) {
      if (!lastEndCharFound) {
        // clear last char
        tft.setTextColor(MAIN_BG);
        tft.drawChar(lastFrequency[i], FREQ_X+i*24, FREQ_Y);
      }
      
      // If last unit string is still on screen, it may intersect new chars -> can only print if display unit is disabled
      // draw new char
      if (!DISPLAY_UNIT) {
        tft.setTextColor(TXT_MAIN);
        tft.drawChar(currentFrequency[i], FREQ_X+i*24, FREQ_Y);
      }      
    }
  }
  //i--;
  j++;

  //if prev is longer then new -> clear the rest
  while (!lastEndCharFound && lastFrequency[j] != '\0') {
    tft.setTextColor(MAIN_BG);
    tft.drawChar(lastFrequency[j], FREQ_X+j*24, FREQ_Y);
    j++;
  }

  /* UNIT */
  // Redraw unit if text length has changed
  if (DISPLAY_UNIT && i != j) {
    tft.setTextSize(1);
    // Clear prev
    tft.setTextColor(MAIN_BG);
    tft.drawString(freqUnit, FREQ_X+j*24, FREQ_Y+10, 4);

    // New unit
    tft.setTextColor(TXT_MAIN);
    tft.drawString(freqUnit, FREQ_X+i*24, FREQ_Y+10, 4);
  }


  /* NEED TO GO BACK TO FREQUENCY, IF DISPLAY UNIT IS ENABLED */
  if (DISPLAY_UNIT) {
    tft.setTextSize(4);
    tft.setTextColor(TXT_MAIN);
    // TODO: It would be enough to print only the missing chars (based on the first for loop), but displaying the whole text wont affect the "user experience" 
    tft.drawString(currentFrequency, FREQ_X, FREQ_Y);
  }
}

void Skin::DisplayRdsPi(const char* currentRdsPi, bool currentSafe, const char* lastRdsPi, bool lastSafe) {
  // No change -> no action
  if (strcmp(currentRdsPi, lastRdsPi) == 0 && currentSafe == lastSafe) return;
  
  /*Serial.print("new PI:");
  Serial.print(currentRdsPi);
  Serial.print(" (stable:");
  Serial.print(currentSafe);
  Serial.println(")");
*/
  /* PI */
  tft.setTextSize(3);
  // Clear prev
  tft.setTextColor(MAIN_BG);
  tft.drawString(lastRdsPi, PI_X, PI_Y);
  // New PI
  if (currentSafe) tft.setTextColor(TXT_MAIN);
  else tft.setTextColor(TXT_UNSAFE);
  tft.drawString(currentRdsPi, PI_X, PI_Y);

}

void Skin::DisplayRdsEcc(const char* currentCountry, const char* lastCountry) {
  // No change -> no action
  if(strcmp(currentCountry, lastCountry) == 0) return;

  /* ECC COUNTRY */
  tft.setTextSize(1);
  // Clear prev
  tft.setTextColor(MAIN_BG);
  for (size_t i = 0; lastCountry[i] != '\0'; ++i) tft.drawChar(toUpperCase(lastCountry[i]), COUNTRY_X+i*6, COUNTRY_Y);
  tft.drawString(lastCountry, COUNTRY_X, COUNTRY_Y);
  // New ECC
  tft.setTextColor(TXT_MAIN);
  for (size_t i = 0; currentCountry[i] != '\0'; ++i) tft.drawChar(toUpperCase(currentCountry[i]), COUNTRY_X+i*6, COUNTRY_Y);
}

void Skin::DisplayRdsPs(const char* currentRdsPs, bool currentSafe, const char* lastRdsPs, bool lastSafe) {
  // No change -> no action
  if (strcmp(currentRdsPs, lastRdsPs) == 0 && currentSafe == lastSafe) return;

  tft.setTextSize(5);
  // Clear
  tft.setTextColor(MAIN_BG);
  tft.drawString(lastRdsPs, PS_X, PS_Y);

  // New PS
  if (currentSafe) tft.setTextColor(TXT_MAIN);
  else tft.setTextColor(TXT_UNSAFE);
  tft.drawString(currentRdsPs, PS_X, PS_Y);
}

void Skin::DisplayRdsRt(const char* currentRdsRt, const char* lastRdsRt) {
  // No change -> no action
  if (strcmp(currentRdsRt, lastRdsRt) == 0) return;

  // Clear
  tft.fillRect(RT_X, RT_Y, SCREEN_WIDTH, 16*RT_LINES, MAIN_BG);
  
  // New RT
  tft.setTextSize(1);
  tft.setTextColor(TXT_MAIN);

  // Split RT into words, lines
  char word[64];
  unsigned wordLength;
  char lines[RT_LINES][40];
  strcpy(lines[0], "");
  strcpy(lines[1], "");
  uint8_t lineIndex = 0;

  while(sscanf(currentRdsRt, " %29s%n", word, &wordLength) == 1) {
    if (tft.textWidth(lines[lineIndex],2)+tft.textWidth(word,2) <= SCREEN_WIDTH-(RT_X*2)) {
      // Add word      
      strcat(lines[lineIndex], word);
      strcat(lines[lineIndex], " ");
      currentRdsRt += wordLength;
    } 
    else {
      // No enough space for word -> new line
      lineIndex++;

      // No more line available -> strip RT
      if (lineIndex == RT_LINES) {
        strcat(lines[lineIndex-1], "...");
        break;
      }
    }
  }

  // Display results
  for (size_t i = 0; i < RT_LINES; i++)
  {
    tft.drawString(lines[i], RT_X, RT_Y+(13*i), 2);
  }
}

void Skin::DisplayEnterFreq(const char* header, const char* freqString, bool drawFrame) {
  if (drawFrame) {
    tft.fillRoundRect(MSGBOX_X+2,MSGBOX_Y+2,MSGBOX_WIDTH+2,MSGBOX_HEIGHT+2,10,TFT_BLACK);
    tft.drawRoundRect(MSGBOX_X,MSGBOX_Y,MSGBOX_WIDTH,MSGBOX_HEIGHT,10,TFT_WHITE);
  }

  tft.fillRoundRect(MSGBOX_X+1,MSGBOX_Y+1,MSGBOX_WIDTH-2,MSGBOX_HEIGHT-2,10,MAIN_BG);

  tft.setTextColor(TXT_MAIN);

  tft.setTextSize(1);
  tft.drawString(header,MSGBOX_X+10,MSGBOX_Y+10,2);
  
  tft.setTextSize(3);
  tft.drawString(freqString,MSGBOX_X+10,MSGBOX_Y+60);
}

void Skin::DisplayMsgBox(const char* header, const char* text, uint8_t headerFont, uint8_t textFont) {
  tft.fillRoundRect(MSGBOX_X+2,MSGBOX_Y+2,MSGBOX_WIDTH+2,MSGBOX_HEIGHT+2,10,TFT_BLACK);
  tft.fillRoundRect(MSGBOX_X,MSGBOX_Y,MSGBOX_WIDTH,MSGBOX_HEIGHT,10,MAIN_BG);
  tft.drawRoundRect(MSGBOX_X,MSGBOX_Y,MSGBOX_WIDTH,MSGBOX_HEIGHT,10,TFT_WHITE);

  tft.setTextColor(TXT_MAIN);
  tft.setTextSize(1);

  tft.drawString(header,MSGBOX_X+10,MSGBOX_Y+10,headerFont);
  
  tft.drawString(text,MSGBOX_X+10,MSGBOX_Y+50,textFont);
}

void Skin::DisplayMenu(char* items[], uint8_t itemCount, uint8_t selectedItem, bool tickedItems[], uint8_t itemsFrom, uint8_t itemsTo) {
  InitMain();
  tft.setTextSize(1);

  for (size_t i = 0; i < MENU_ITEMS; i++)
  {
    uint8_t rowY = HEADER_HEIGHT+1+i*((SCREEN_HEIGHT-HEADER_HEIGHT-1)/MENU_ITEMS);
    tft.fillRect(0,rowY,SCREEN_WIDTH,(SCREEN_HEIGHT-HEADER_HEIGHT-1)/MENU_ITEMS,i%2 == 0 ? MAIN_BG : HEADER_BG);
    
    if (i+itemsFrom < itemCount) {
      uint32_t txtColor = i+itemsFrom == selectedItem ? MAIN_BG : TXT_MAIN; 
      uint8_t padding = i+itemsFrom == selectedItem ? 10 : 0;
      tft.setTextColor(txtColor);

      // Select frame
      if (i+itemsFrom == selectedItem) tft.fillRoundRect(0,rowY,SCREEN_WIDTH,(SCREEN_HEIGHT-HEADER_HEIGHT-1)/MENU_ITEMS,10,TXT_MAIN);
      
      // Title
      tft.drawString(items[itemsFrom+i],MENU_TXT_X+padding,rowY+MENU_TXT_Y,2);

      // Checkbox
      if(tickedItems[itemsFrom+i]) {
        tft.drawRect(MENU_CHECK_X,rowY+MENU_CHECK_Y,MENU_CHECK_SIZE,MENU_CHECK_SIZE,txtColor);
        tft.drawLine(
          MENU_CHECK_X+3,
          rowY+MENU_CHECK_Y+(MENU_CHECK_SIZE/2)+2,
          MENU_CHECK_X+(MENU_CHECK_SIZE/2),
          rowY+MENU_CHECK_Y+MENU_CHECK_SIZE-2,
          txtColor);
        tft.drawLine(
          MENU_CHECK_X+2,
          rowY+MENU_CHECK_Y+(MENU_CHECK_SIZE/2)+2,
          MENU_CHECK_X+(MENU_CHECK_SIZE/2)-1,
          rowY+MENU_CHECK_Y+MENU_CHECK_SIZE-2,
          txtColor);
        tft.drawLine(
          MENU_CHECK_X+MENU_CHECK_SIZE-2,
          rowY+MENU_CHECK_Y+2,
          MENU_CHECK_X+(MENU_CHECK_SIZE/2),
          rowY+MENU_CHECK_Y+MENU_CHECK_SIZE-2,
          txtColor);
        tft.drawLine(
          MENU_CHECK_X+MENU_CHECK_SIZE-3,
          rowY+MENU_CHECK_Y+2,
          MENU_CHECK_X+(MENU_CHECK_SIZE/2)-1,
          rowY+MENU_CHECK_Y+MENU_CHECK_SIZE-2,
          txtColor);
      }
    }
  }
  
}