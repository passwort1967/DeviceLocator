#pragma once

// Libs
#include "Arduino.h"
#include "settings.h"
#include "structs.h"

class Locator {
public:
  static State state;
  static Network networks[MAX_WIFI];
  static Network *foundNetworks[MAX_WIFI];
  static Vector devicePosition;
  static bool newDevicePosition;
  static float distanceThreshold;
  static int networkCount;
  static int foundCount;
  static int scanCount;

  static void setup();
  static bool addWifi(String name, float x, float y, int maxStrength);
  static void removeAllWifi();

  // Main Fuctionality
  static void update();
  static void locate();

  // Debug
  static void debugSearch();
  static void debugNetworks();
  static void debugFoundNetworks();
  static void debugPosition();

private:
  static void search();

  // Calculations
  static void calculateDistance();
  static void triangulate();

  // Helper
  static Intersections calculateIntersectionPoints(Network N0, Network N1);
  static Vector calculateCenter(Intersections *intersections,
                                int numIntersections);
  static void growDistancesForIntersections(Network *n0, Network *n1);
  static int calculateNumPermutations(int numNetworks);
  static float calculateDistanceBetweenPoints(Vector p0, Vector p2);
  static void updateState();
};
