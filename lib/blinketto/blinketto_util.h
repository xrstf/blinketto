#pragma once

#include <stdint.h> // uint8_t etc.

bool setUint8ConfigValue(uint8_t *target, char *args, uint8_t min, uint8_t max);
bool setUint32ConfigValue(uint32_t *target, char *args, uint32_t min, uint32_t max);
