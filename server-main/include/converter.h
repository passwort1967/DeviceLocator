#pragma once

#include "Arduino.h"

class Convert {
public:
  static const int intSize;
  static const int floatSize;

  static void intToByte(int input, byte *output);
  static int byteToInt(byte *input);

  static void floatToByte(float input, byte *output);
  static float byteToFloat(byte *input);
};