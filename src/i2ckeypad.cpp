/*
 *  i2ckeypad.cpp v0.1 - keypad/I2C expander interface for Arduino
 *
 *  Copyright (c) 2009 Angel Sancho <angelitodeb@gmail.com>
 *  All rights reserved.
 *
 *  Original source from keypad v0.3 of Mark Stanley <mstanley@technologist.com>
 *  (http://www.arduino.cc/playground/Main/KeypadTutorial)
 *
 *
 *  LICENSE
 *  -------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  EXPLANATION
 *  -----------
 *  This library is designed for use with PCF8574, but can possibly be
 *  adapted to other I2C port expanders
 *
 *  Wiring diagrams for PCF8574 and 4x3 keypad can be found under
 *  examples directory. Library runs correctly without cols pull-up
 *  resistors but it's better to use it
 *
 *  You can change pin connections between PCF8574 and keypad under
 *  PIN MAPPING section below
 *
 *  IMPORTANT! You have to call Wire.begin() before init() in your code
 *
 *  ... and sorry for my poor english!
 */

#include "i2ckeypad.h"
#include <Wire.h>

/*
extern "C" {
  #include "WConstants.h"
}
*/

/*
 *  PIN MAPPING
 *
 *  Here you can change your wire mapping between your keypad and PCF8574
 *  Default mapping is for sparkfun 4x3 keypad
 */

#define COL0            4 
#define COL1            5  
#define COL2            6 
#define COL3            7  
#define ROW0            3  
#define ROW1            2 
#define ROW2            1  
#define ROW3            0 
#define HOLD_COUNT      10
#define PUSH_TOLERANCE  1   // must be at least 1


/*
 *  KEYPAD KEY MAPPING
 *
 *  Default key mapping for 4x4 keypads, you can change it here if you have or
 *  like different keys
 */

const char keymap[4][4] =
{
  {1,2,3,KEY_MATRIX_A},
  {4,5,6,KEY_MATRIX_B},
  {7,8,9,KEY_MATRIX_C},
  {KEY_MATRIX_ASTERISK,0,KEY_MATRIX_SHARP,KEY_MATRIX_D}
};


/*
 *  VAR AND CONSTANTS DEFINITION. Don't change nothing here
 *
 */

// Default row and col pin counts
int num_rows = 4;
int num_cols = 4;

// Last states
char lastButton;
uint8_t lastButtonCount;

// PCF8574 i2c address
int pcf8574_i2c_addr;

// Hex byte statement for each port of PCF8574
const int hex_data[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

// Hex data for each row of keypad in PCF8574
const int pcf8574_row_data[4] = 
{
  hex_data[ROW1] | hex_data[ROW2] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW2] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW1] | hex_data[ROW3] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
  hex_data[ROW0] | hex_data[ROW1] | hex_data[ROW2] |
  hex_data[COL0] | hex_data[COL1] | hex_data[COL2] | hex_data[COL3],
};

// Hex data for each col of keypad in PCF8574
int col[4] = {hex_data[COL0], hex_data[COL1], hex_data[COL2], hex_data[COL3]};


/*
 *  CONSTRUCTORS
 */

i2ckeypad::i2ckeypad(int addr)
{
  pcf8574_i2c_addr = addr;
}

i2ckeypad::i2ckeypad(int addr, int r, int c)
{
  pcf8574_i2c_addr = addr;
  num_rows = r;
  num_cols = c;
}


/*
 *  PUBLIC METHODS
 */

void i2ckeypad::init()
{
  // All PCF8574 ports high
  pcf8574_write(pcf8574_i2c_addr, 0xff);
}

bool i2ckeypad::get_key(char *key, bool *hold)
{
  bool same = false;
  bool found = false;
  char tempBtn = '\0';
  int tmp_data;
  
  // Get current char
  for (size_t i = 0; i < num_rows; i++)
  {
    pcf8574_write(pcf8574_i2c_addr, pcf8574_row_data[i]);

    // Read pcf8574 port data
    tmp_data = pcf8574_byte_read(pcf8574_i2c_addr);

    // XOR to compare obtained data and current data and know
    tmp_data ^= pcf8574_row_data[i];

    if (tmp_data) {
      int column = uint8_t(log2(tmp_data >> 4));
      // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
      tempBtn = keymap[i][3-column];
 /*     Serial.println("...........");
      Serial.println(col[column],BIN);
      Serial.println(pcf8574_row_data[i],BIN);
      Serial.println(tmp_data ^= pcf8574_row_data[i],BIN);
*/
      if (tempBtn == lastButton) lastButtonCount++;
      break;
    }
  }

  // Is it changed?
  if (tempBtn == lastButton) same = true;

  // Is hold?
  *hold = lastButtonCount >= HOLD_COUNT;
  if (*hold) {
    //Serial.print("HOLD: ");
    //Serial.println(lastButtonCount);

    *key = tempBtn;
    found = true;
  } 

  // Press = pressed for short time, then released
  if (!same && lastButtonCount >= PUSH_TOLERANCE && !found) {
    //Serial.print("trigger: ");
    *key = lastButton;
    *hold = false;
    found = true;
  }

  lastButton = tempBtn;
  if (!same) lastButtonCount = 0;

  // All PCF8574 ports high again
  pcf8574_write(pcf8574_i2c_addr, 0xff);

  return found;
}




/* SIMPLIER */
bool i2ckeypad::get_key_v2(char *key)
{
  bool found = false;
  int tmp_data;
  
  // Get current char
  for (size_t i = 0; i < num_rows; i++)
  {
    pcf8574_write(pcf8574_i2c_addr, pcf8574_row_data[i]);

    // Read pcf8574 port data
    tmp_data = pcf8574_byte_read(pcf8574_i2c_addr);

    // XOR to compare obtained data and current data and know
    tmp_data ^= pcf8574_row_data[i];

    if (tmp_data) {
      int column = uint8_t(log2(tmp_data >> 4));
      // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
      *key = keymap[i][3-column];
      found = true;
      break;
    }
  }

  // All PCF8574 ports high again
  pcf8574_write(pcf8574_i2c_addr, 0xff);
  
  return found;
}









/*
 *  PRIVATE METHODS
 */

void i2ckeypad::pcf8574_write(int addr, int data)
{
  Wire.beginTransmission(addr);
  Wire.write(data);
  Wire.endTransmission();
}

int i2ckeypad::pcf8574_byte_read(int addr)
{
  Wire.requestFrom(addr, 1);

  return Wire.read();
}