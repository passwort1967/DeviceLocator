#include "resultHandler.h"
#include "ArduinoLog.h"

// Globals
int VectorHandler::smoothingRange;
int VectorHandler::currentResultIndex;
int VectorHandler::nextResultIndex;
int VectorHandler::numStoredResults;
Vector VectorHandler::results[MAX_STORED_RESULTS];

// Config
void VectorHandler::setup() {
  VectorHandler::smoothingRange = 1;
  VectorHandler::currentResultIndex = 0;
  VectorHandler::nextResultIndex = 0;
  VectorHandler::numStoredResults = 0;

  Log.infoln("Vector Handler Ready \n");
}

void VectorHandler::setSmoothingRange(int smoothingRange) {
  if (smoothingRange > MAX_STORED_RESULTS) {
    smoothingRange = MAX_STORED_RESULTS;
  }
  if (smoothingRange < 1) {
    smoothingRange = 1;
  }
  VectorHandler::smoothingRange = smoothingRange;
}

// API
void VectorHandler::add(Vector result) {
  VectorHandler::results[VectorHandler::nextResultIndex] = result;
  VectorHandler::currentResultIndex = VectorHandler::nextResultIndex;

  VectorHandler::nextResultIndex++;
  VectorHandler::numStoredResults++;

  if (VectorHandler::numStoredResults > MAX_STORED_RESULTS) {
    VectorHandler::numStoredResults = MAX_STORED_RESULTS;
  }

  if (VectorHandler::nextResultIndex >= MAX_STORED_RESULTS) {
    VectorHandler::nextResultIndex = 0;
  }
}

bool VectorHandler::getSmoothedVector(Vector *out) {
  if (VectorHandler::numStoredResults == 0) {
    return false;
  }

  int range = VectorHandler::smoothingRange;

  if (range > VectorHandler::numStoredResults) {
    range = VectorHandler::numStoredResults;
  }

  int idx;
  for (int i = 0; i < VectorHandler::smoothingRange; i++) {
    idx = VectorHandler::currentResultIndex - i;

    if (idx < 0) {
      idx += MAX_STORED_RESULTS;
    }

    out->x += VectorHandler::results[idx].x;
    out->y += VectorHandler::results[idx].y;
  }

  out->x /= range;
  out->y /= range;

  return true;
}

void VectorHandler::clear() {
  VectorHandler::smoothingRange = 1;
  VectorHandler::currentResultIndex = 0;
  VectorHandler::nextResultIndex = 0;
  VectorHandler::numStoredResults = 0;
}

// Debugging
void VectorHandler::printResults() {
  Log.verboseln("Total Stored Results: %d \n", VectorHandler::numStoredResults);
  Log.verboseln("Current Result Index: %d \n",
                VectorHandler::currentResultIndex);
  Log.verboseln("Current Result: (%f,%f)\n",
                VectorHandler::results[VectorHandler::currentResultIndex].x,
                VectorHandler::results[VectorHandler::currentResultIndex].y);

  for (int i = 0; i < MAX_STORED_RESULTS; i++) {
    Vector *v = &VectorHandler::results[i];
    Log.verboseln("Vector at idx %d: (%f,%f)\n", i, v->x, v->y);
  }
}