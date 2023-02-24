#include "server.h"
#include "ArduinoJson.h"
#include "ArduinoLog.h"
#include "WiFi.h"
#include "communication.h"
#include "resultHandler.h"
#include "settings.h"
#include "structs.h"

// Globals
WebServer TriangulationServer::server(80);

void TriangulationServer::setup() {
  TriangulationServer::connectToWifi();

  TriangulationServer::server.on("/clear", HTTP_GET,
                                 TriangulationServer::handleClear);
  TriangulationServer::server.on("/add", HTTP_POST,
                                 TriangulationServer::handleAdd);
  TriangulationServer::server.on("/config", HTTP_POST,
                                 TriangulationServer::handleConfig);
  TriangulationServer::server.on("/locate", HTTP_GET,
                                 TriangulationServer::handleLocate);
  TriangulationServer::server.onNotFound(TriangulationServer::handleNotFound);

  TriangulationServer::server.begin();

  Log.infoln("Triangulation Server Ready \n");
}

void TriangulationServer::update() {
  TriangulationServer::checkWifiConnection();
  TriangulationServer::server.handleClient();
}

// Routing
void TriangulationServer::handleClear() {
  Log.infoln("Received Clear Command \n");

  Config c;
  c.controll = CLEAR;
  Com::send(c);

  TriangulationServer::server.send(200);
  VectorHandler::clear();
}

void TriangulationServer::handleAdd() {
  Log.infoln("Received Add Command");

  if (TriangulationServer::server.hasArg("plain") == false) {
    Log.warningln("Request is missing Body\n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No Body Received");
    return;
  }

  String body = TriangulationServer::server.arg("plain");

  StaticJsonDocument<192> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Log.warningln("Deserialisation Error \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "Deserialisation Error");
    return;
  }

  if (!doc.containsKey("name")) {
    Log.warningln("No Name Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No Name Field");
    return;
  }

  if (!doc.containsKey("maxStrength")) {
    Log.warningln("No MaxStrength Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No MaxStrength Field");
    return;
  }

  if (!doc.containsKey("X")) {
    Log.warningln("No X Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No X Field");
    return;
  }

  if (!doc.containsKey("Y")) {
    Log.warningln("No Y Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No Y Field");
    return;
  }

  Config c;
  c.controll = ADD;
  c.name = String((const char *)doc["name"]);
  c.maxStrength = doc["maxStrength"];
  c.x = doc["X"];
  c.y = doc["Y"];

  Log.verboseln("Name: %s", c.name);
  Log.verboseln("Max Strength: %d", c.maxStrength);
  Log.verboseln("X: %F", c.x);
  Log.verboseln("Y: %F", c.y);

  Com::send(c);

  Log.infoln("");

  TriangulationServer::server.send(200);
}

void TriangulationServer::handleConfig() {
  Log.infoln("Received Config Command");

  if (TriangulationServer::server.hasArg("plain") == false) {
    Log.warningln("Request is missing Body\n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No Body Received");
    return;
  }

  String body = TriangulationServer::server.arg("plain");

  StaticJsonDocument<96> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    Log.warningln("Deserialisation Error \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "Deserialisation Error");
    return;
  }

  if (!doc.containsKey("smoothingRange")) {
    Log.warningln("No SmoothingRange Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No SmoothingRange Field");
    return;
  }

  if (!doc.containsKey("distanceThreshold")) {
    Log.warningln("No DistanceThreshold Field \n");
    TriangulationServer::server.send(HTTP_BAD_REQUEST, "plain/text",
                                     "No DistanceThreshold Field");
    return;
  }

  VectorHandler::setSmoothingRange(doc["smoothingRange"]);

  Config c;
  c.controll = CONFIGURE;
  c.distanceThreshold = doc["distanceThreshold"];

  Log.verboseln("Smoothing Range: %d", doc["smoothingRange"]);
  Log.verboseln("Distance Threshold: %d", doc["distanceThreshold"]);

  Com::send(c);

  Log.infoln("");

  TriangulationServer::server.send(200);
}

void TriangulationServer::handleLocate() {
  Log.infoln("Received Locate Command");

  Vector result;
  if (!VectorHandler::getSmoothedVector(&result)) {
    Log.infoln("No Position calculatet yet \n");
    TriangulationServer::server.send(HTTP_NO_CONTENT, "plain/text",
                                     "No Position calculatet yet");
    return;
  }

  String body;
  StaticJsonDocument<32> doc;

  doc["X"] = result.x;
  doc["Y"] = result.y;

  Log.verboseln("Position Sent: (%F, %F)", result.x, result.y);

  serializeJson(doc, body);

  Log.infoln("");

  TriangulationServer::server.send(HTTP_OK, "application/json", body);
}

void TriangulationServer::handleNotFound() {
  Log.warningln("Received Unknown Request \n");
  TriangulationServer::server.send(404);
}

// Helper
void TriangulationServer::connectToWifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  Log.infoln("Wifi Connected");
  Log.infoln("IP Address: %s \n", WiFi.localIP().toString().c_str());
}

void TriangulationServer::checkWifiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Log.infoln("Connection Lost. Reconnecting");

  do {
    WiFi.reconnect();
    delay(100);
  } while (WiFi.status() != WL_CONNECTED);

  Log.infoln("Reconnected");
  Log.infoln("IP Address: %s \n", WiFi.localIP().toString().c_str());
}