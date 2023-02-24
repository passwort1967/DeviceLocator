#include "main.h"

// Libs
#include "Arduino.h"
#include "ArduinoLog.h"
#include "communication.h"
#include "locator.h"
#include "settings.h"
#include "structs.h"

// Main Methods
void setup() {
  // Setups
  Serial.begin(MONITOR_SPEED);
  Log.begin(LOG_LEVEL, &Serial, false);

  Locator::setup();
  SlaveCom::setup();

  Serial.println();
  Serial.println();

  Log.infoln("Locator\n");
}

void loop() {
  processNewConfigs();
  Locator::update();
  Locator::debugFoundNetworks();

  Locator::locate();
  Locator::debugPosition();
}

// Helper
void processNewConfigs() {
  int configCount = 0;
  Config configs[MESSAGE_QUEUE_SIZE];
  Config *con;

  SlaveCom::getMessages(configs, &configCount);

  for (int i = 0; i < configCount; i++) {
    con = &configs[i];

    switch (con->controll) {
    case Controll::ADD: {
      // Add new network
      Locator::addWifi(con->name, con->x, con->y, con->maxStrength);
      break;
    }
    case Controll::CLEAR: {
      // Clear all networks
      Locator::removeAllWifi();
      break;
    }
    case Controll::CONFIGURE: {
      Locator::distanceThreshold = con->distanceThreshold;
      break;
    }
    }
  }
}
