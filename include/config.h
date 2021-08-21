#pragma once

#include <stdint.h>

// informational purpose only
#define BLINKETTO_VERSION "3.0"

// set to 0 if there is no status LED on the board
#define LED_PIN 5

// GPIO pin for the sleep mode button
#define SLEEP_BUTTON_PIN 4

// pins for the PISO shift register
#define SHIFTREGISTER_LOAD_PIN 13
#define SHIFTREGISTER_DATA_PIN 16
#define SHIFTREGISTER_CLK_PIN  12
// #define SHIFTREGISTER_CLKINH_PIN 6 // always low in board 4.0 rev1

// Only some of the variants maintained in https://github.com/esp8266/Arduino/tree/master/variants
// have the correct pinout for an ESP-WROOM-02. To keep the code generic,
// we define our own constants. In the Arduino IDE, just choose "ESP8266 Generic".
#define I2C_SDA_PIN 2  // this is GPIO2, on physical pin 7
#define I2C_SCK_PIN 14 // this is GPIO14, on physical pin 3
#define I2C_ADDRESS 0x15

#define MODE_NONE        0
#define MODE_FULL_ON     1
#define MODE_BLINK       2
#define MODE_COUNTER     3
#define MODE_RANDOM_FADE 4
#define MODE_KITT        5
