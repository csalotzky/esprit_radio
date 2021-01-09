#ifndef i2ckeypad_h
#define i2ckeypad_h

#include <inttypes.h>
#include "Arduino.h"
#include "definitions.h"

class i2ckeypad {
public:
  i2ckeypad(int);
  i2ckeypad(int, int, int);
  bool get_key(char *key);
  void init();
  
private:
  void pcf8574_write(int, int);
  int pcf8574_byte_read(int);
};

#endif

