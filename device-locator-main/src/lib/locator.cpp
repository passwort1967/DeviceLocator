#include "locator.h"

// Libs
#include "ArduinoLog.h"
#include "WiFi.h"

// Static Variables
State Locator::state;
Network Locator::networks[MAX_WIFI];
Network *Locator::foundNetworks[MAX_WIFI];
Vector Locator::devicePosition;
bool Locator::newDevicePosition;
float Locator::distanceThreshold;
int Locator::networkCount;
int Locator::foundCount;
int Locator::scanCount;

// Configuration
void Locator::setup() {
  networkCount = 0;
  scanCount = 0;
  foundCount = 0;
  newDevicePosition = false;
  distanceThreshold = CLOSE_MATCH_THRESHOLD;
  Locator::state = CONFIGURATION;

  WiFi.mode(WIFI_STA);
  WiFi.setScanMethod(WIFI_SCAN_METHOD);
  WiFi.disconnect();
  delay(100);
}

bool Locator::addWifi(String name, float x, float y, int maxStrengt) {
  Log.infoln("Add new Wifi");

  // Check if MAX_WIFI is reached
  if (Locator::networkCount >= MAX_WIFI) {
    Log.warningln("Maximum of Wifi Networks reached. Maximum: %d \n", MAX_WIFI);
    return false;
  }

  Log.verboseln("Name: %s", name.c_str());
  Log.verboseln("Max Strength: %d", maxStrengt);
  Log.verboseln("X: %F", x);
  Log.verboseln("Y: %F", y);

  Network *n;

  // Check if wifi is allready added
  for (int i = 0; i < Locator::networkCount; i++) {
    n = &Locator::networks[i];
    if (n->name.equals(name)) {
      Log.verboseln("Overriding existing Network \n");
      n->position.x = x;
      n->position.y = y;
      n->found = false;
      n->maxStrength = maxStrengt;
      return true;
    }
  }

  // Add new wifi
  n = &Locator::networks[Locator::networkCount];

  n->name = name;
  n->position.x = x;
  n->position.y = y;
  n->maxStrength = maxStrengt;
  n->strength = INT_MIN;
  n->found = false;

  Locator::networkCount++;

  Locator::updateState();

  Serial.println();
  return true;
}

void Locator::removeAllWifi() {
  Log.infoln("Remove all Wifis \n");

  for (int i = 0; i < MAX_WIFI; i++) {
    Locator::networks[i] = Network();
  }

  Locator::networkCount = 0;
  Locator::scanCount = 0;
  Locator::foundCount = 0;

  Locator::updateState();
}

// Main Functionality
void Locator::update() {
  Log.infoln("Update");
  Log.verboseln("Network Count: %d", Locator::networkCount);
  Log.verboseln("Found Count: %d", Locator::foundCount);

  if (Locator::state == CONFIGURATION) {
    Log.verboseln("Not Updating \n");
    Locator::updateState();
    return;
  }

  Log.verboseln("Updating");
  Serial.println();

  Locator::search();
  Locator::updateState();
}

void Locator::locate() {
  Log.infoln("Locate");
  Log.verboseln("Network Count: %d", Locator::networkCount);
  Log.verboseln("Found Count: %d", Locator::foundCount);

  if (Locator::state != LOCALISATION) {
    Log.verboseln("Not Locating \n");
    return;
  }

  Log.verboseln("Locating");
  Serial.println();

  Locator::calculateDistance();
  Locator::triangulate();
}

// Main Functionality
void Locator::search() {
  WiFi.scanDelete();

  Locator::scanCount = WiFi.scanNetworks();
  Locator::foundCount = 0;

  Network *n;
  for (int i = 0; i < Locator::networkCount; i++) {
    n = &Locator::networks[i];
    n->found = false;

    for (int w = 0; w < Locator::scanCount; w++) {
      String wifiName = WiFi.SSID(w);

      if (n->name.equals(wifiName)) {
        n->strength = WiFi.RSSI(w);
        n->found = true;

        Locator::foundNetworks[foundCount] = n;

        Locator::foundCount++;
        break;
      }
    }
  }
}

void Locator::calculateDistance() {
  for (int i = 0; i < Locator::foundCount; i++) {
    Network *n = Locator::foundNetworks[i];

    // Strength Loss: Positive difference between received signal and max signal
    int strengthLoss = (n->strength - n->maxStrength) * -1;

    float exponent = (WIFI_DISTANCE_CONSTANT - (20 * log10f(WIFI_FREQUENCY)) +
                      strengthLoss) /
                     20.0f;

    n->distance = powf(10, exponent);
  }
}

void Locator::triangulate() {

  Intersections *intersections =
      new Intersections[calculateNumPermutations(foundCount)];

  int intersectionCount = 0;
  for (int fist = 0; fist < foundCount - 1; fist++) {
    for (int second = fist + 1; second < foundCount; second++) {
      intersections[intersectionCount] = calculateIntersectionPoints(
          *foundNetworks[fist], *foundNetworks[second]);

      intersectionCount++;
    }
  }

  Locator::devicePosition = calculateCenter(intersections, intersectionCount);
  Locator::newDevicePosition = true;
  delete[] intersections;
}

// Calculations
Vector Locator::calculateCenter(Intersections *intersections,
                                int numIntersections) {

  Vector **currentMatches = new Vector *[numIntersections * 2];
  Vector **newMatches = new Vector *[numIntersections * 2];
  Vector **temp;

  int currentMatchCount = 0;
  int newMatchCount = 0;
  float distance = 0.0f;

  // Filter Relevant Points
  for (int i0count = 0; i0count < numIntersections - 1; i0count++) {
    Intersections *i0 = &intersections[i0count];

    for (int p0count = 0; p0count < i0->pointsFound; p0count++) {
      Vector *p0 = &(i0->points[p0count]);

      newMatchCount = 0;
      newMatches[newMatchCount] = p0;
      newMatchCount += 1;

      for (int i1count = i0count + 1; i1count < numIntersections; i1count++) {
        Intersections *i1 = &intersections[i1count];

        for (int p1count = 0; p1count < i1->pointsFound; p1count++) {
          Vector *p1 = &(i1->points[p1count]);

          distance = calculateDistanceBetweenPoints(*p0, *p1);

          if (distance <= Locator::distanceThreshold) {
            newMatches[newMatchCount] = p1;
            newMatchCount += 1;
          }
        }
      }

      if (newMatchCount > currentMatchCount) {
        temp = currentMatches;
        currentMatches = newMatches;
        newMatches = temp;
        currentMatchCount = newMatchCount;
      }
    }
  }

  // Calculate Center (Centroid) of relevant points;
  Vector center;

  for (int i = 0; i < currentMatchCount; i++) {
    center.x += currentMatches[i]->x;
    center.y += currentMatches[i]->y;
  }

  center.x /= currentMatchCount;
  center.y /= currentMatchCount;

  delete[] currentMatches;
  delete[] newMatches;

  return center;
}

Intersections Locator::calculateIntersectionPoints(Network N0, Network N1) {
  growDistancesForIntersections(&N0, &N1);

  Intersections ip;
  Vector P0, P1, SymIntersect, UnitVecX, UnitVecY;
  float r0, r1, c, dx, dy = 0.0f;

  // Unpack information
  P0 = N0.position;
  P1 = N1.position;
  r0 = N0.distance;
  r1 = N1.distance;
  ip.pointsFound = 0;

  // Calculate Distance between two Circles Middlepoints
  dx = P1.x - P0.x;
  dy = P1.y - P0.y;

  c = sqrtf(dx * dx + dy * dy);

  if (c == 0.0) {
    // Middlepoints are identical
    return ip;
  }

  // Calculate x and y. Easy because of symmetrical Assumption of System
  SymIntersect.x = (r0 * r0 + c * c - r1 * r1) / (2 * c);
  SymIntersect.y = (r0 * r0) - (SymIntersect.x * SymIntersect.x);

  if (SymIntersect.y < 0.0) {
    // No intersections
    return ip;
  }

  if (SymIntersect.y > 0.0) {
    SymIntersect.y = sqrtf(SymIntersect.y);
  }

  // Calculate unit vectors
  UnitVecX.x = dx / c;
  UnitVecX.y = dy / c;

  UnitVecY.x = UnitVecX.y * -1;
  UnitVecY.y = UnitVecX.x;

  // Transform Assumption of symmetrical System to real System

  ip.points[0].x = P0.x + SymIntersect.x * UnitVecX.x;
  ip.points[0].y = P0.y + SymIntersect.x * UnitVecX.y;

  if (SymIntersect.y == 0.0) {
    // Circles are Touching
    ip.pointsFound = 1;
    return ip;
  }

  ip.points[1].x = ip.points[0].x - SymIntersect.y * UnitVecY.x;
  ip.points[1].y = ip.points[0].y - SymIntersect.y * UnitVecY.y;

  ip.points[0].x += SymIntersect.y * UnitVecY.x;
  ip.points[0].y += SymIntersect.y * UnitVecY.y;

  ip.pointsFound = 2;

  return ip;
}

// Helper
void Locator::growDistancesForIntersections(Network *n0, Network *n1) {
  float radicant, pointDistance, totalRadius = 0.0;
  pointDistance = calculateDistanceBetweenPoints(n0->position, n1->position);
  totalRadius = n0->distance + n1->distance;

  while (totalRadius < pointDistance) {
    n1->distance += WIFI_DISTANCE_GROW_STEP;
    n0->distance += WIFI_DISTANCE_GROW_STEP;

    totalRadius = n0->distance + n1->distance;
  }
}

float Locator::calculateDistanceBetweenPoints(Vector p0, Vector p1) {
  float dx = p1.x - p0.x;
  float dy = p1.y - p0.y;

  return sqrtf((dx * dx) + (dy * dy));
}

int Locator::calculateNumPermutations(int numNetworks) {
  int ret = 0;
  for (int i = 0; i < numNetworks; i++) {
    ret += i;
  }

  return ret;
}

void Locator::updateState() {
  switch (Locator::state) {
  case CONFIGURATION: {
    if (Locator::networkCount >= NUM_NETWORKS_NECESSARY) {
      Locator::state = SEARCH;
    }
    break;
  }
  case SEARCH: {
    if (Locator::networkCount < NUM_NETWORKS_NECESSARY) {
      Locator::state = CONFIGURATION;
      break;
    }
    if (Locator::foundCount >= NUM_NETWORKS_NECESSARY) {
      Locator::state = LOCALISATION;
    }
    break;
  }
  case LOCALISATION: {
    if (Locator::networkCount < NUM_NETWORKS_NECESSARY) {
      Locator::state = CONFIGURATION;
      break;
    }
    if (Locator::foundCount < NUM_NETWORKS_NECESSARY) {
      Locator::state = SEARCH;
    }
    break;
  }
  }
}

// Debugging
void Locator::debugSearch() {
  if (Log.getLevel() != LOG_LEVEL_VERBOSE) {
    return;
  }

  Log.verboseln("Debug Search");

  if (Locator::scanCount == 0) {
    Log.verboseln("Networks received: %d \n", Locator::scanCount);
    return;
  }

  Log.verboseln("Networks received: %d", Locator::scanCount);

  for (int i = 0; i < Locator::scanCount; i++) {
    Log.verboseln("Name: %s", WiFi.SSID(i).c_str());
    Log.verboseln("Strengt: %d \n", WiFi.RSSI(i));
  }
}

void Locator::debugNetworks() {
  if (Log.getLevel() != LOG_LEVEL_VERBOSE) {
    return;
  }

  Log.verboseln("Debug Configured Networks");

  if (Locator::networkCount == 0) {
    Log.verboseln("Networks Configured: %d \n", Locator::networkCount);
    return;
  }

  Log.verboseln("Networks Configured: %d", Locator::networkCount);

  Network *n;
  for (int i = 0; i < Locator::networkCount; i++) {
    n = &Locator::networks[i];

    Log.verboseln("Name: %s", n->name.c_str());
    Log.verboseln("Found: %T", n->found);
    Log.verboseln("Max Strengt %d", n->maxStrength);
    Log.verboseln("Curren Strength %d", n->strength);
    Log.verboseln("Distance %F", n->distance);
    Log.verboseln("Position: (%F|%F) \n", n->position.x, n->position.y);
  }
}

void Locator::debugFoundNetworks() {
  if (Log.getLevel() != LOG_LEVEL_VERBOSE) {
    return;
  }

  Log.verboseln("Debug Found / Mapped Networks");
  if (Locator::foundCount == 0) {
    Log.verboseln("Networks Found and Mapped: %d \n ", Locator::foundCount);
    return;
  }
  Log.verboseln("Networks Found and Mapped: %d ", Locator::foundCount);

  Network *n;
  for (int i = 0; i < Locator::foundCount; i++) {
    n = Locator::foundNetworks[i];

    Log.verboseln("Name: %s", n->name.c_str());
    Log.verboseln("Found: %T", n->found);
    Log.verboseln("Max Strengt %d", n->maxStrength);
    Log.verboseln("Curren Strength %d", n->strength);
    Log.verboseln("Distance %F", n->distance);
    Log.verboseln("Position: (%F|%F) \n", n->position.x, n->position.y);
  }
}

void Locator::debugPosition() {
  if (Log.getLevel() != LOG_LEVEL_VERBOSE) {
    return;
  }

  Log.verboseln("Debug Device Position");
  Log.verboseln("Position: (%F|%F) \n", Locator::devicePosition.x,
                Locator::devicePosition.y);
}