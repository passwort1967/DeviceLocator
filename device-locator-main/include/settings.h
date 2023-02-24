#pragma once

// General Settings
#define MONITOR_SPEED 921600
#define LOG_LEVEL LOG_LEVEL_VERBOSE
#define DEVICE_ID 0

// Serial Settings
#define SERIAL_OUT_MESSAGE_LENGTH 200
#define SERIAL_IN_MESSAGE_LENGTH 200
#define RESULT_DOC_SIZE 64
#define CONFIG_DOC_SIZE 64
#define SERIAL_IN_MESSAGE_LENGTH 200

// Communication
#define CHUNK_SIZE 128
#define MESSAGE_QUEUE_SIZE 10
#define SLAVE_ADR 1
#define SEND_TIMEOUT 100
#define SDA 32
#define SCL 14

// Wifi Settings
#define MAX_WIFI 5
#define WIFI_FREQUENCY 2400.0f
#define WIFI_DISTANCE_CONSTANT 27.55f
#define WIFI_SCAN_METHOD WIFI_FAST_SCAN
#define WIFI_DISTANCE_GROW_STEP 0.1f

// Trilateration
#define CLOSE_MATCH_THRESHOLD 1.8f
#define NUM_NETWORKS_NECESSARY 3

// Pins
#define BUTTON_1_PIN 23

// LED
#define LED_0_PIN 22
#define LED_1_PIN 21
#define LED_2_PIN 19
#define LED_3_PIN 18
#define LED_4_PIN 5
#define LED_5_PIN 17
#define LED_6_PIN 16
#define LED_7_PIN 4
#define LED_8_PIN 2
#define LED_9_PIN 15
#define FLASH_DURATION 200
