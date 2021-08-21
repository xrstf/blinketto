#pragma once

#include <stdint.h>

void setupLED();
void setLED(bool enabled);
void enableLED();
void disableLED();

const char *modeName(uint8_t mode);
uint8_t modeConst(const char *name);
