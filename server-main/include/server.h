#pragma once

#include "WebServer.h"

class TriangulationServer {
public:
  static void setup();
  static void update();

private:
  static WebServer server;
  static void connectToWifi();
  static void checkWifiConnection();

  // Routing
  static void handleClear();
  static void handleAdd();
  static void handleConfig();
  static void handleLocate();
  static void handleNotFound();
};