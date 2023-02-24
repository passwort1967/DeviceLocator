#include "main.h"
#include "ArduinoLog.h"
#include "Wire.h"
#include "communication.h"
#include "resultHandler.h"
#include "server.h"
#include "settings.h"
#include "structs.h"
#include <Arduino.h>

void setup() {
  Serial.begin(MONITOR_SPEED);
  Log.begin(LOG_LEVEL, &Serial, false);

  Log.infoln("\nServer \n");

  // Setups
  TriangulationServer::setup();
  Com::setup();
  VectorHandler::setup();
}

void loop() {
  fetchNewLocation();
  TriangulationServer::update();
  delay(300);
}

// Helper
void fetchNewLocation() {
  Result result;

  Com::request(&result);

  // Filter invalid results
  if (result.newVal != true) {
    return;
  }
  if (isnan(result.x) || isnan(result.y)) {
    return;
  }

  // Add new result
  Vector vec;
  vec.x = result.x;
  vec.y = result.y;

  VectorHandler::add(vec);

  Log.infoln("Added new Position\n");
}