#include "driver.h"
#include "util.h"
#include <blinketto.h>
#include <cstring>
#include <xrstf_util.h>

PCA9624Driver::PCA9624Driver(uint8_t i2cAddress)
	: driver(i2cAddress), mode(BLINKETTO_NO_MODE), ledStates{0}, brightnesses{0} {
	char autoinc = PCA9624_AI_OFF;
	char sleep   = 0; // the only flag we actually want to change
	char sub1    = 0;
	char sub2    = 0;
	char sub3    = 0;
	char allcall = 0;
	char addr    = PCA9624_ADDR_CHIP;

	this->driver.writeMode1(autoinc, sleep, sub1, sub2, sub3, allcall, addr);

	// the PCA remembers the individual brightness levels
	// when our MCU reboots, so make sure to clean that up
	this->setAll(BLKO_BRIGHTNESS_OFF);
}

void PCA9624Driver::setAll(uint8_t brightness) {
	uint8_t brightnesses[BLKO_MAX_OUTPUTS];
	memset(brightnesses, brightness, BLKO_MAX_OUTPUTS);
	this->setBrightnesses(brightnesses);
}

void PCA9624Driver::turnOff() {
	this->setMode(BLKO_MODE_OFF);
}

void PCA9624Driver::turnOn(uint8_t brightness) {
	this->setAll(brightness);
}

bool PCA9624Driver::setBrightness(uint8_t led, uint8_t brightness) {
	if (led >= BLKO_MAX_OUTPUTS) {
		return false;
	}

	this->setMode(BLKO_MODE_INDIVIDUAL);
	this->driver.writePWM(led, brightness, PCA9624_ADDR_CHIP);
	this->brightnesses[led] = brightness;

	return true;
}

void PCA9624Driver::setBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) {
	this->setMode(BLKO_MODE_INDIVIDUAL);
	this->driver.writeAllPWMs((char *)brightnesses, 0, 0);
	memcpy(this->brightnesses, brightnesses, sizeof(uint8_t) * BLKO_MAX_OUTPUTS);
}

void PCA9624Driver::setLEDStates(uint8_t newState) {
	this->ledStates = newState;
	this->turnOff();
}

uint8_t PCA9624Driver::getLEDStates() {
	return this->ledStates;
}

void PCA9624Driver::getBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) {
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		brightnesses[i] = this->brightnesses[i];
	}
}

void PCA9624Driver::setMode(uint8_t mode) {
	if (mode != this->mode) {
		uint8_t pcaMode = PCA9624_LED_OFF;

		switch (mode) {
		case BLKO_MODE_OFF:
			pcaMode = PCA9624_LED_OFF;
			break;

		case BLKO_MODE_ON:
			pcaMode = PCA9624_LED_ON_NO_CONTROL;
			break;

		case BLKO_MODE_INDIVIDUAL:
			pcaMode = PCA9624_LED_INDIVIDUAL;
			break;
		}

		this->writeLedOutputState(pcaMode);
		mode = this->mode;
	}
}

void PCA9624Driver::writeLedOutputState(uint8_t active, uint8_t inactive) {
	this->driver.writeLedOut(CHECK_BIT(this->ledStates, 0) ? active : inactive, CHECK_BIT(this->ledStates, 1) ? active : inactive, CHECK_BIT(this->ledStates, 2) ? active : inactive, CHECK_BIT(this->ledStates, 3) ? active : inactive,
																																																			CHECK_BIT(this->ledStates, 4) ? active
																																																										  : inactive,
																																																			CHECK_BIT(this
																																																																																																																																																							->ledStates,
																																																																																																					5)
																																																																																																					? active
																																																																																																					: inactive,
																																																			CHECK_BIT(this->ledStates, 6)
																																																																																																					? active
																																																																																																					: inactive,
																																																			CHECK_BIT(this->ledStates, 7) ? active
																																																										  : inactive);
}
