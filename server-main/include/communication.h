#pragma once

#include "Arduino.h"
#include "structs.h"

class Com {
public:
  static void setup();
  static void send(Config config);
  static void request(Result *res);
};
