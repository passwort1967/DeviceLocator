#include "communication.h"

// Libs
#include "ArduinoLog.h"
#include "Wire.h"
#include "settings.h"

byte receiveBuffer[MESSAGE_SIZE] = {0};

void Com::setup() {
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();
  Wire.setClock(10000);

  while (true) {
    Wire.beginTransmission(SLAVE_ADR);

    if (Wire.endTransmission(true) == 0) {
      break;
    }

    delay(500);
  }

  Log.infoln("Connected To Client\n");
}

void Com::send(Config config) {
  byte sendBuffer[MESSAGE_SIZE];

  config.toByteArray(sendBuffer);

  Wire.beginTransmission(SLAVE_ADR);
  Wire.write(sendBuffer, MESSAGE_SIZE);
  Wire.endTransmission();

  delay(SEND_TIMEOUT);
}

void Com::request(Result *res) {
  int receivedBytes = Wire.requestFrom(SLAVE_ADR, MESSAGE_SIZE);

  Wire.readBytes(receiveBuffer, MESSAGE_SIZE);

  res->fromByteArray(receiveBuffer);
}
