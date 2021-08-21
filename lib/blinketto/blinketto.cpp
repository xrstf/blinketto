#include "blinketto.h"
#include <Arduino.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

uint8_t BlinkettoDriver::numActiveLEDs() {
	uint8_t active = 0;
	uint8_t state  = this->getLEDStates();

	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		if (CHECK_BIT(state, i))
			active++;
	}

	return active;
}

void BlinkettoDriver::transition(uint8_t targetBrightnesses[BLKO_MAX_OUTPUTS], uint8_t steps, uint32_t stepDelay) {
	if (steps > 200) {
		steps = 200;
	}

	uint8_t originalBrightnesses[BLKO_MAX_OUTPUTS] = {0};
	float stepSizes[BLKO_MAX_OUTPUTS]              = {0.0};

	this->getBrightnesses(originalBrightnesses);

	// calculate step sizes
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		uint8_t current = originalBrightnesses[i];
		uint8_t target  = targetBrightnesses[i];

		stepSizes[i] = (target - current) / (float)steps;
	}

	this->setMode(BLKO_MODE_INDIVIDUAL);

	// perform all required steps
	for (uint8_t step = 1; step <= steps; ++step) {
		uint8_t newBrightnesses[BLKO_MAX_OUTPUTS];

		// calculate the new brightnesses for each LED
		for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
			float brightness = (float)originalBrightnesses[i];
			float stepSize   = stepSizes[i];

			brightness += step * stepSize;

			newBrightnesses[i] = (uint8_t)brightness;
		}

		this->setBrightnesses(newBrightnesses);
		delay(stepDelay);
	}
}

DummyBlinkettoDriver::DummyBlinkettoDriver() {
	this->ledStates = 0x00;
	this->mode      = BLKO_MODE_UNDEFINED;

	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		this->brightnesses[i] = 0x00;
	}
}

void DummyBlinkettoDriver::setMode(uint8_t mode) {
	printf("# set LED mode to %d\n", mode);
	this->mode = mode;
}

void DummyBlinkettoDriver::setAll(uint8_t brightness) {
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		this->brightnesses[i] = brightness;
	}
	this->setMode(BLKO_MODE_INDIVIDUAL);
	this->update();
}

void DummyBlinkettoDriver::turnOff() {
	printf("# turned all LEDs off\n");
	this->setMode(BLKO_MODE_OFF);
}

void DummyBlinkettoDriver::turnOn(uint8_t brightness) {
	printf("# turned all LEDs on\n");
	uint8_t brightnesses[BLKO_MAX_OUTPUTS];
	memset(brightnesses, brightness, BLKO_MAX_OUTPUTS);
	this->setBrightnesses(brightnesses);
}

bool DummyBlinkettoDriver::setBrightness(uint8_t led, uint8_t brightness) {
	printf("# hardware.setBrightness(%d, 0x%x)\n", led, brightness);

	if (led >= BLKO_MAX_OUTPUTS) {
		return false;
	}

	this->brightnesses[led] = brightness;
	this->setMode(BLKO_MODE_INDIVIDUAL);
	this->update();

	return true;
}

void DummyBlinkettoDriver::setBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) {
	printf("# hardware.setBrightnesses(");
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		this->brightnesses[i] = brightnesses[i];

		printf("0x%x", brightnesses[i]);
		if (i < BLKO_MAX_OUTPUTS - 1) {
			printf(",");
		}
	}
	printf(")\n");
	this->setMode(BLKO_MODE_INDIVIDUAL);
	this->update();
}

void DummyBlinkettoDriver::update() {
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		printf("%d", this->brightnesses[i]);

		if (i < BLKO_MAX_OUTPUTS - 1) {
			printf(";");
		}
	}

	printf("\n");
}

void DummyBlinkettoDriver::setLEDStates(uint8_t newState) {
	printf("# new LED state 0b%s\n", xrstf::fmtBinary(newState));
	this->ledStates = newState;
	// this->turnOff();
}

uint8_t DummyBlinkettoDriver::getLEDStates() {
	return this->ledStates;
}

void DummyBlinkettoDriver::getBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) {
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		brightnesses[i] = this->brightnesses[i];
	}
}

Blinketto::Blinketto(BlinkettoDriver &driver, uint8_t fullBrightness) : driver(driver), fullBrightness(fullBrightness) {
}

void Blinketto::setAll(uint8_t brightness) {
	this->driver.setAll(brightness);
}

void Blinketto::turnOff() {
	this->driver.turnOff();
}

void Blinketto::turnOn(uint8_t percentage) {
	this->driver.turnOn(this->mapPercentage(percentage));
}

uint8_t Blinketto::numActiveLEDs() {
	return this->driver.numActiveLEDs();
}

void Blinketto::getBrightnesses(uint8_t percentages[]) {
	uint8_t hwBrightnesses[BLKO_MAX_OUTPUTS] = {0};
	this->driver.getBrightnesses(hwBrightnesses);
	uint8_t ledStates = this->driver.getLEDStates();

	uint8_t pos = 0;
	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		if (CHECK_BIT(ledStates, i)) {
			percentages[pos] = this->mapBrightness(hwBrightnesses[i]);
			pos++;
		}
	}
}

uint8_t Blinketto::mapPercentage(uint8_t percentage) {
	return mapPercentageToMax(percentage, this->fullBrightness);
}

uint8_t mapPercentageToMax(uint8_t percentage, uint8_t max) {
	if (percentage == 0) {
		return 0;
	}

	if (percentage >= 100) {
		return max;
	}

	float onePercent = max / 100.0;

	return uint8_t(onePercent * percentage);
}

uint8_t Blinketto::mapBrightness(uint8_t brightness) {
	if (brightness == 0) {
		return 0;
	}

	if (brightness >= this->fullBrightness) {
		return 100;
	}

	float percentage = (brightness * 100.0) / (float)this->fullBrightness;

	return uint8_t(percentage);
}

bool Blinketto::setBrightness(uint8_t led, uint8_t percentage) {
	// printf("# logical.setBrightness(%d, %x%%)\n", led, percentage);

	uint8_t ledStates = this->driver.getLEDStates();
	uint8_t activePos = 0; // keep track of active LEDs by increasing this counter

	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		if (CHECK_BIT(ledStates, i)) {
			if (led == activePos) {
				return this->driver.setBrightness(i, this->mapPercentage(percentage));
			}
			activePos++;
		}
	}

	return false;
}

void Blinketto::mapPercentages(uint8_t brightnessPercentages[], uint8_t result[]) {
	uint8_t ledStates = this->driver.getLEDStates();
	uint8_t activePos = 0; // keep track of active LEDs by increasing this counter

	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		// plugged in
		if (CHECK_BIT(ledStates, i)) {
			result[i] = this->mapPercentage(brightnessPercentages[activePos]);
			activePos++;
		} else {
			result[i] = 0;
		}
	}
}

void Blinketto::setBrightnesses(uint8_t brightnessPercentages[]) {
	// printf("# logical.setBrightnesses()\n");

	uint8_t mapped[BLKO_MAX_OUTPUTS];
	this->mapPercentages(brightnessPercentages, mapped);

	this->driver.setBrightnesses(mapped);
}

void Blinketto::transition(uint8_t brightnessPercentages[], uint8_t steps, uint32_t stepDelay) {
	// printf("# logical.transition(%d)\n", steps);

	uint8_t mapped[BLKO_MAX_OUTPUTS];
	this->mapPercentages(brightnessPercentages, mapped);

	// for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
	// 	xrstf::serialPrintf("Blinketto.transition#mapped[%d] = %d\n", i, mapped[i]);
	// }

	this->driver.transition(mapped, steps, stepDelay);
}

void Blinketto::transitionToBitmap(uint8_t bitmap, uint8_t steps, uint8_t brightnessPercent, uint32_t stepDelay) {
	uint8_t brightnesses[BLKO_MAX_OUTPUTS]; // easier than dynamically allocating just the right amount

	for (uint8_t i = 0; i < BLKO_MAX_OUTPUTS; ++i) {
		brightnesses[i] = CHECK_BIT(bitmap, i) ? brightnessPercent : 0;
		// xrstf::serialPrintf("Blinketto.transitionToBitmap#brightness[%d] = %d\n", i, brightnesses[i]);
	}

	this->transition(brightnesses, steps, stepDelay);
}

// NB: Do not mix up the naming here. We map [0..100%] to a value [0..255],
// regardless of the configured fullBrightness. In all other spots,
// brightnessPercentage always refers to [0..fullBrightness].
void Blinketto::setFullBrightness(uint8_t brightnessPercentage) {
	this->fullBrightness = mapPercentageToMax(brightnessPercentage, 0xFF);
}
