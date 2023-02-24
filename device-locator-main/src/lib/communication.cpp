#include "communication.h"

// Libs
#include "ArduinoLog.h"
#include "Wire.h"
#include "locator.h"
#include "settings.h"

// Static Vars
byte SlaveCom::messageQueue[MESSAGE_QUEUE_SIZE][CHUNK_SIZE];
int SlaveCom::messageQueueLength = 0;

// Public
void SlaveCom::setup() {
  Wire.setPins(SDA, SCL);
  Wire.begin(SLAVE_ADR);
  Wire.onReceive(receive);
  Wire.onRequest(request);

  Log.verboseln("SlaveCom Setup");
  Log.verboseln("Adress: %d", SLAVE_ADR);
  Log.verboseln("Pins - SDA: %d, SCL: %d \n", SDA, SCL);
}

bool SlaveCom::getMessages(Config *configOutput, int *configCount) {
  if (messageQueueLength == 0) {
    return false;
  }

  // Type Config
  *configCount = extractConfigMessages(configOutput);

  // Other Message Types
  messageQueueLength = 0;
  return true;
}

// Callbacks
void SlaveCom::receive(int receivedBytes) {
  Log.verboseln("Received %d Bytes \n", receivedBytes);

  if (receivedBytes != CHUNK_SIZE) {
    return;
  }

  if (messageQueueLength >= MESSAGE_QUEUE_SIZE) {
    return;
  }

  Wire.readBytes(messageQueue[messageQueueLength], CHUNK_SIZE);
  messageQueueLength++;
}

void SlaveCom::request() {
  Log.verboseln("Received Request \n");

  Result res;
  res.id = SLAVE_ADR;
  res.newVal = Locator::newDevicePosition;
  res.x = Locator::devicePosition.x;
  res.y = Locator::devicePosition.y;

  byte resutBuffer[CHUNK_SIZE];
  res.toByteArray(resutBuffer);

  Wire.write(resutBuffer, CHUNK_SIZE);
  Locator::newDevicePosition = false;
}

// Helper
int SlaveCom::extractConfigMessages(Config *configOutput) {
  int configCount = 0;

  for (int i = 0; i < messageQueueLength; i++) {
    byte *message = messageQueue[i];
    configOutput[configCount].fromByteArray(message);
    configCount++;
  }

  return configCount;
}
