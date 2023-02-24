#pragma once

// Serial
#define MONITOR_SPEED 921600
#define LOG_LEVEL LOG_LEVEL_VERBOSE

// Communication
#define MESSAGE_SIZE 128
#define SLAVE_ADR 1
#define SEND_TIMEOUT 10

// Pins
#define SDA_PIN 23
#define SCL_PIN 19

// ResultHandler
#define MAX_STORED_RESULTS 10

// WebServer
#define WIFI_SSID "iPhone von Philippe"
#define WIFI_PASSWORD "duKommschHierNedRein"

// HTTP Status Code
#define HTTP_OK 200
#define HTTP_BAD_REQUEST 400
#define HTTP_NO_CONTENT 204