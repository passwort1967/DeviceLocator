#pragma once

#include "Arduino.h"
#include "settings.h"
#include "structs.h"

class SlaveCom {
public:
  static float x;
  static float y;

  static void setup();
  static bool getMessages(Config *configOutput, int *configCount);

private:
  static byte messageQueue[MESSAGE_QUEUE_SIZE][CHUNK_SIZE];
  static int messageQueueLength;

  static void receive(int receivedBytes);
  static void request();
  static int extractConfigMessages(Config *configOutput);
};
