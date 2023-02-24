#pragma once

// Libs
#include "Arduino.h"
#include "converter.h"
#include "settings.h"

enum State { CONFIGURATION, SEARCH, LOCALISATION };
enum Controll { CLEAR, ADD, CONFIGURE };

struct Vector {
  float x;
  float y;

  Vector() {
    this->x = 0.0f;
    this->y = 0.0f;
  }

  Vector(float x, float y) {
    this->x = x;
    this->y = y;
  }
};

struct Network {
  // Given Atrributes
  String name;
  int maxStrength;

  // Position
  Vector position;

  // Calculated Attributes
  int strength;
  float distance;
  bool found;

  Network() {
    this->name = "";
    this->maxStrength = 0;
    this->position = Vector();
    this->strength = 0;
    this->distance = 0.0f;
    this->found = false;
  }
};

struct Intersections {
  int pointsFound;
  Vector points[2];
};

struct Config {
  Controll controll;
  float distanceThreshold;
  int maxStrength;

  float x;
  float y;

  String name;

  void toByteArray(byte *byteOutput) {
    int bytesWritten = 0;

    // Controll
    int controll = static_cast<int>(this->controll);
    Convert::intToByte(controll, &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // Distance Threshod
    Convert::floatToByte(this->distanceThreshold, &byteOutput[bytesWritten]);
    bytesWritten += Convert::floatSize;

    // Max Strength
    Convert::intToByte(this->maxStrength, &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // X
    Convert::floatToByte(this->x, &byteOutput[bytesWritten]);
    bytesWritten += Convert::floatSize;

    // Y
    Convert::floatToByte(this->y, &byteOutput[bytesWritten]);
    bytesWritten += Convert::floatSize;

    // Name
    int bytesLeft = CHUNK_SIZE - bytesWritten;

    if (bytesLeft > this->name.length()) {
      for (int i = 0; i < this->name.length(); i++) {
        byteOutput[bytesWritten] = this->name.charAt(i);
        bytesWritten++;
      }
      for (int i = bytesWritten; i < CHUNK_SIZE; i++) {
        byteOutput[bytesWritten] = '\0';
        bytesWritten++;
      }
    }

    else {
      for (int i = 0; i < bytesLeft - 1; i++) {
        byteOutput[bytesWritten] = this->name.charAt(i);
        bytesWritten++;
      }
      byteOutput[bytesWritten] = '\0';
      bytesWritten++;
    }

    Serial.println(bytesWritten);
  }

  void fromByteArray(byte *byteInput) {
    int bytesRead = 0;

    // Controll
    int controll = Convert::byteToInt(&byteInput[bytesRead]);
    this->controll = static_cast<Controll>(controll);
    bytesRead += Convert::intSize;

    // Distance Threshold
    this->distanceThreshold = Convert::byteToFloat(&byteInput[bytesRead]);
    bytesRead += Convert::floatSize;

    // Max Strrength
    this->maxStrength = Convert::byteToInt(&byteInput[bytesRead]);
    bytesRead += Convert::intSize;

    // X
    this->x = Convert::byteToFloat(&byteInput[bytesRead]);
    bytesRead += Convert::floatSize;

    // Y
    this->y = Convert::byteToFloat(&byteInput[bytesRead]);
    bytesRead += Convert::floatSize;

    // name
    int bytesLeft = CHUNK_SIZE - bytesRead;
    this->name = String("");
    char charBuf;

    for (int i = 0; i < bytesLeft; i++) {
      charBuf = byteInput[bytesRead];

      if (charBuf == '\0') {
        break;
      }

      this->name.concat(charBuf);
      bytesRead++;
    }
  }
};

struct Result {
  int id;
  bool newVal;
  float x;
  float y;

  int networkCount;
  int foundCount;
  State state;

  void toByteArray(byte *byteOutput) {
    int bytesWritten = 0;

    // ID
    Convert::intToByte(this->id, &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // New Val
    byteOutput[bytesWritten] = this->newVal;
    bytesWritten++;

    // X
    Convert::floatToByte(this->x, &byteOutput[bytesWritten]);
    bytesWritten += Convert::floatSize;

    // Y
    Convert::floatToByte(this->y, &byteOutput[bytesWritten]);
    bytesWritten += Convert::floatSize;

    // Network Count
    Convert::intToByte(this->networkCount, &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // Found Count
    Convert::intToByte(this->foundCount, &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // State
    Convert::intToByte(static_cast<int>(this->state),
                       &byteOutput[bytesWritten]);
    bytesWritten += Convert::intSize;

    // Fill with Endbytes
    int bytesLeft = CHUNK_SIZE - bytesWritten;
    for (int i = 0; i < bytesLeft; i++) {
      byteOutput[bytesWritten] = '\0';
      bytesWritten++;
    }
  }

  void fromByteArray(byte *byteInput) {
    int bytesRead = 0;

    // ID
    this->id = Convert::byteToInt(&byteInput[bytesRead]);
    bytesRead += Convert::intSize;

    // New Val
    this->newVal = byteInput[bytesRead];
    bytesRead++;

    // X
    this->x = Convert::byteToFloat(&byteInput[bytesRead]);
    bytesRead += Convert::floatSize;

    // Y
    this->y = Convert::byteToFloat(&byteInput[bytesRead]);
    bytesRead += Convert::floatSize;

    // Network Count
    this->networkCount = Convert::byteToInt(&byteInput[bytesRead]);
    bytesRead += Convert::intSize;

    // Found Count
    this->foundCount = Convert::byteToInt(&byteInput[bytesRead]);
    bytesRead += Convert::intSize;

    // State
    int state = Convert::byteToInt(&byteInput[bytesRead]);
    this->state = static_cast<State>(state);
    bytesRead += Convert::intSize;
  }
};
