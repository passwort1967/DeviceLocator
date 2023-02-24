#pragma once

#include "settings.h"
#include "structs.h"

class VectorHandler {
public:
  // Config
  static void setup();
  static void setSmoothingRange(int smoothingRange);

  // API
  static void add(Vector result);
  static void clear();
  static bool getSmoothedVector(Vector *out);

  // Debugging
  static void printResults();

private:
  static int smoothingRange;
  static int currentResultIndex;
  static int nextResultIndex;
  static int numStoredResults;
  static Vector results[MAX_STORED_RESULTS];
};
