#include "converter.h"

const int Convert::intSize = 4;
const int Convert::floatSize = 4;

void Convert::intToByte(int input, byte *output) {
  output[0] = (input >> 24);
  output[1] = (input >> 16);
  output[2] = (input >> 8);
  output[3] = input;
}

int Convert::byteToInt(byte *input) {
  return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
}

void Convert::floatToByte(float input, byte *output) {
  // OMG WHY C++, WHYYYYYYY ????
  int i = *(int *)&input;

  output[0] = i & 0x00FF;
  output[1] = (i >> 8) & 0x00FF;
  output[2] = (i >> 16) & 0x00FF;
  output[3] = (i >> 24);

  // intToByte(i, output);
}

float Convert::byteToFloat(byte *input) {
  // OMG WHY C++, WHYYYYYYY Part 2 ????
  int i = input[0] | (input[1] << 8) | (input[2] << 16) | (input[3] << 24);
  return *(float *)&i;
}
