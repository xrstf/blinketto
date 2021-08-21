#include "blinketto_util.h"
#include <Arduino.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

bool setUint8ConfigValue(uint8_t *target, char *args, uint8_t min, uint8_t max) {
	if (strlen(args) == 0) {
		Serial.println("Error: A number must be provided.");
		return false;
	}

	if (strlen(args) > 3) {
		Serial.println("Error: Number must be at most 3 digits long.");
		return false;
	}

	int value = 0;
	if (!xrstf::safeStringToInt(args, &value)) {
		Serial.println("Error: Invalid number given.");
		return false;
	}

	if (value < min || value > max) {
		xrstf::serialPrintf("Error: Number must be [%d..%d].\n", min, max);
		return false;
	}

	*target = (uint8_t)value;
	return true;
}

bool setUint32ConfigValue(uint32_t *target, char *args, uint32_t min, uint32_t max) {
	if (strlen(args) == 0) {
		Serial.println("Error: A number must be provided.");
		return false;
	}

	if (strlen(args) > 8) {
		Serial.println("Error: Number must be at most 8 digits long.");
		return false;
	}

	int value = 0;
	if (!xrstf::safeStringToInt(args, &value)) {
		Serial.println("Error: Invalid number given.");
		return false;
	}

	if (value < 0) {
		xrstf::serialPrintf("Error: Number must be [%d..%d].\n", min, max);
		return false;
	}

	if ((uint32_t)value < min || (uint32_t)value > max) {
		xrstf::serialPrintf("Error: Number must be [%d..%d].\n", min, max);
		return false;
	}

	*target = (uint32_t)value;
	return true;
}
