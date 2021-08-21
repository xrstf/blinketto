#pragma once

#include <stdint.h>

class PlugManager {
public:
	PlugManager(uint8_t clockPin, uint8_t loadPin, uint8_t dataPin);

	uint8_t getStatus();

private:
	uint8_t clockPin;
	uint8_t loadPin;
	uint8_t dataPin;
};
