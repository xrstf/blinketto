#include "plugman.h"
#include <Arduino.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

PlugManager::PlugManager(uint8_t clockPin, uint8_t loadPin, uint8_t dataPin)
	: clockPin(clockPin), loadPin(loadPin), dataPin(dataPin) {
	pinMode(this->clockPin, OUTPUT);
	pinMode(this->loadPin, OUTPUT);
	pinMode(this->dataPin, INPUT);
}

uint8_t PlugManager::getStatus() {
	// setup
	digitalWrite(this->loadPin, HIGH);
	digitalWrite(this->clockPin, LOW);

	// start reading
	uint8_t ret = 0;

	// clock in the data
	digitalWrite(this->loadPin, LOW);
	digitalWrite(this->loadPin, HIGH);

	for (uint8_t i = 0; i < 8; ++i) {
		uint8_t value = digitalRead(this->dataPin);
		SET_BIT_TO_VALUE(ret, i, !value);

		digitalWrite(this->clockPin, HIGH);
		digitalWrite(this->clockPin, LOW);
	}

	return xrstf::flipLSB(ret, 8);
}
