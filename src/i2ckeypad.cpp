/**********************
* TODO! OUT OF ORDER! *
***********************/

#include "i2ckeypad.h"
#include <Wire.h>

/* PIN MAPS */

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


/* KEY MAPPING */

const char keymap[4][4] =
{
  {1,2,3,KEY_MATRIX_A},
  {4,5,6,KEY_MATRIX_B},
  {7,8,9,KEY_MATRIX_C},
  {KEY_MATRIX_ASTERISK,0,KEY_MATRIX_SHARP,KEY_MATRIX_D}
};

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

void i2ckeypad::init() {
  // All PCF8574 ports high
  pcf8574_write(pcf8574_i2c_addr, 0xff);
}

bool i2ckeypad::get_key(char *key)
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