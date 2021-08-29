#include "blinketto_mode_counter.h"
#include "blinketto_util.h"
#include <stdint.h> // uint8_t etc.
#include <stdio.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

BlinkettoModeCounter::BlinkettoModeCounter(BlinkettoModeCounterConfig *config) : config(config) {
}

bool BlinkettoModeCounter::canRun(Blinketto &blinketto) {
	return true;
}

void BlinkettoModeCounter::setConfigDefaults(BlinkettoModeCounterConfig *config) {
	config->steps              = 10;
	config->stepDelay          = 2;
	config->timeBetweenNumbers = 1000;
	config->timeBetweenEndings = 2000;
}

void BlinkettoModeCounter::toJSON(BlinkettoModeCounterConfig *config, JsonDocument &doc) {
	doc["steps"] = config->steps;
	doc["delay"] = config->stepDelay;
	doc["nInt"] = config->timeBetweenNumbers; // "number interval"
	doc["eInt"] = config->timeBetweenEndings; // "ending interval"
}

bool BlinkettoModeCounter::updateFromJSON(BlinkettoModeCounterConfig *config, const JsonDocument &doc) {
	bool changes = false;

	if (doc.containsKey("steps")) {
		int steps = doc["steps"];
		if (steps >= 1 && steps <= 50 && ((char)steps) != config->steps) {
			xrstf::serialPrintf("Update: Setting modeCounter.steps to %d.\n", steps);
			config->steps = (uint8_t)steps;
			changes       = true;
		}
	}

	if (doc.containsKey("delay")) {
		int stepDelay = doc["delay"];
		if (stepDelay >= 0 && stepDelay <= 100 && ((char)stepDelay) != config->stepDelay) {
			xrstf::serialPrintf("Update: Setting modeCounter.stepDelay to %d.\n", stepDelay);
			config->stepDelay = (uint32_t)stepDelay;
			changes           = true;
		}
	}

	if (doc.containsKey("nInt")) {
		int timeBetweenNumbers = doc["nInt"];
		if (timeBetweenNumbers >= 0 && timeBetweenNumbers <= 10000 && ((char)timeBetweenNumbers) != config->timeBetweenNumbers) {
			xrstf::serialPrintf("Update: Setting modeCounter.timeBetweenNumbers to %d.\n", timeBetweenNumbers);
			config->timeBetweenNumbers = (uint32_t)timeBetweenNumbers;
			changes                    = true;
		}
	}

	if (doc.containsKey("eInt")) {
		int timeBetweenEndings = doc["eInt"];
		if (timeBetweenEndings >= 0 && timeBetweenEndings <= 10000 && ((char)timeBetweenEndings) != config->timeBetweenEndings) {
			xrstf::serialPrintf("Update: Setting modeCounter.timeBetweenEndings to %d.\n", timeBetweenEndings);
			config->timeBetweenEndings = (uint32_t)timeBetweenEndings;
			changes                    = true;
		}
	}

	return changes;
}

bool BlinkettoModeCounter::handleCLICommand(BlinkettoModeCounterConfig *config, char *cmdArgs) {
	if (xrstf::startsWith(cmdArgs, "steps ")) {
		return setUint8ConfigValue(&config->steps, cmdArgs + strlen("steps "), 1, 50);
	}

	if (xrstf::startsWith(cmdArgs, "stepDelay ")) {
		return setUint32ConfigValue(&config->stepDelay, cmdArgs + strlen("stepDelay "), 0, 100);
	}

	if (xrstf::startsWith(cmdArgs, "timeBetweenNumbers ")) {
		return setUint32ConfigValue(&config->timeBetweenNumbers, cmdArgs + strlen("timeBetweenNumbers "), 0, 10000);
	}

	if (xrstf::startsWith(cmdArgs, "timeBetweenEndings ")) {
		return setUint32ConfigValue(&config->timeBetweenEndings, cmdArgs + strlen("timeBetweenEndings "), 0, 10000);
	}

	xrstf::serialPrintf("Error: Unknown sub command '%s'.\n", cmdArgs);
	return false;
}

/*

reverse decrement means counting down, but flipping the bits
to "count down from the other side", to make it look fancy

...
00011010   26
00011011   27
00011100   28
00011101   29
00011110   30

- top -
00011111   31
- top -

00001111   15
00010111   23
00000111    7
00011011   27
00001011   11
*/

uint8_t reverseDecrement(uint8_t value, uint8_t bitSize) {
	return xrstf::flipLSB(xrstf::flipLSB(value, bitSize) - 1, bitSize);
}

void BlinkettoModeCounter::run(Blinketto &blinketto, BlinkettoInterrupter &interrupter) {
	uint8_t activeLEDs = blinketto.numActiveLEDs();

	// this is [0..255] for activeLEDs=8, but to prevent loops
	// from overflowing and introducing subtle bugs,
	// we store this as a 16 bit value
	uint16_t maxValue = (1U << activeLEDs) - 1;

	while (true) {
		uint16_t currentValue;

		// we start at 1 because we do not want to "count to zero" as the first step
		for (currentValue = 1; currentValue <= maxValue; ++currentValue) {
			blinketto.transitionToBitmap(currentValue, this->config->steps, 100, this->config->stepDelay);
			delay(this->config->timeBetweenNumbers);
			STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
		}

		delay(this->config->timeBetweenEndings);
		STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)

		// count down to zero again
		do {
			currentValue = reverseDecrement(currentValue, activeLEDs);
			blinketto.transitionToBitmap(currentValue, this->config->steps, 100, this->config->stepDelay);
			delay(this->config->timeBetweenNumbers);
			STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
		} while (currentValue > 0);

		delay(this->config->timeBetweenEndings);
		STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
	}
}
