#include "blinketto_mode_random_fade.h"
#include "blinketto_util.h"
#include <stdint.h> // uint8_t etc.
#include <stdio.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

BlinkettoModeRandomFade::BlinkettoModeRandomFade(BlinkettoModeRandomFadeConfig *config) : config(config) {
}

bool BlinkettoModeRandomFade::canRun(Blinketto &blinketto) {
	return true;
}

void BlinkettoModeRandomFade::setConfigDefaults(BlinkettoModeRandomFadeConfig *config) {
	config->steps                = 10;
	config->stepDelay            = 10;
	config->minChangePerInterval = 25;
	config->maxChangePerInterval = 50;
}

bool BlinkettoModeRandomFade::updateFromJSON(BlinkettoModeRandomFadeConfig *config, const JsonDocument &doc) {
	bool changes = false;

	if (doc.containsKey("steps")) {
		int steps = doc["steps"];
		if (steps >= 0 && steps <= 60 && ((char)steps) != config->steps) {
			xrstf::serialPrintf("Update: Setting modeRandomFade.steps to %d.\n", steps);
			config->steps = (uint8_t)steps;
			changes       = true;
		}
	}

	if (doc.containsKey("stepDelay")) {
		int stepDelay = doc["stepDelay"];
		if (stepDelay >= 0 && stepDelay <= 60 && ((char)stepDelay) != config->stepDelay) {
			xrstf::serialPrintf("Update: Setting modeRandomFade.stepDelay to %d.\n", stepDelay);
			config->stepDelay = (uint32_t)stepDelay;
			changes           = true;
		}
	}

	if (doc.containsKey("minChangePerInterval")) {
		int minChangePerInterval = doc["minChangePerInterval"];
		if (minChangePerInterval >= 0 && minChangePerInterval <= 60 && ((char)minChangePerInterval) != config->minChangePerInterval) {
			xrstf::serialPrintf("Update: Setting modeRandomFade.minChangePerInterval to %d.\n", minChangePerInterval);
			config->minChangePerInterval = (uint8_t)minChangePerInterval;
			changes                      = true;
		}
	}

	if (doc.containsKey("maxChangePerInterval")) {
		int maxChangePerInterval = doc["maxChangePerInterval"];
		if (maxChangePerInterval >= 0 && maxChangePerInterval <= 60 && ((char)maxChangePerInterval) != config->maxChangePerInterval) {
			xrstf::serialPrintf("Update: Setting modeRandomFade.maxChangePerInterval to %d.\n", maxChangePerInterval);
			config->maxChangePerInterval = (uint8_t)maxChangePerInterval;
			changes                      = true;
		}
	}

	return changes;
}

bool BlinkettoModeRandomFade::handleCLICommand(BlinkettoModeRandomFadeConfig *config, char *cmdArgs) {
	if (xrstf::startsWith(cmdArgs, "steps ")) {
		return setUint8ConfigValue(&config->steps, cmdArgs + strlen("steps "), 1, 50);
	}

	if (xrstf::startsWith(cmdArgs, "stepDelay ")) {
		return setUint32ConfigValue(&config->stepDelay, cmdArgs + strlen("stepDelay "), 0, 100);
	}

	if (xrstf::startsWith(cmdArgs, "minChangePerInterval ")) {
		return setUint8ConfigValue(&config->minChangePerInterval, cmdArgs + strlen("minChangePerInterval "), 1, 100);
	}

	if (xrstf::startsWith(cmdArgs, "maxChangePerInterval ")) {
		return setUint8ConfigValue(&config->maxChangePerInterval, cmdArgs + strlen("maxChangePerInterval "), 1, 100);
	}

	xrstf::serialPrintf("Error: Unknown sub command '%s'.\n", cmdArgs);
	return false;
}

void BlinkettoModeRandomFade::run(Blinketto &blinketto, BlinkettoInterrupter &interrupter) {
	uint8_t activeLEDs = blinketto.numActiveLEDs();

	// waste a few elements for this array instead of dynamically
	// allocating just the right amount (activeLEDs)
	uint8_t percentages[BLKO_MAX_OUTPUTS] = {0};
	blinketto.getBrightnesses(percentages);

	while (true) {
		// calculate new percentages for each active LED
		this->calculateNewValues(percentages, activeLEDs);
		blinketto.transition(percentages, this->config->steps, this->config->stepDelay);
		STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
		yield();
	}
}

void BlinkettoModeRandomFade::calculateNewValues(uint8_t *values, uint8_t numValues) {
	for (uint8_t i = 0; i < numValues; ++i) {
		uint8_t change = this->config->minChangePerInterval +
		                 random(this->config->maxChangePerInterval - this->config->minChangePerInterval);
		uint8_t direction  = random(100);
		uint8_t percentage = values[i];

		// the brighter we are, the less likely it should be
		// to add even more brightness; this prevents LEDs
		// from getting stuck and makes direction changes
		// less likely (compared to a simple coin flip)
		if (direction > percentage) { // get brighter
			percentage += change;
			if (percentage > 100) {
				percentage = 100;
			}
		} else { // get dimmer
			if (change > percentage) {
				percentage = 0;
			} else {
				percentage -= change;
			}
		}

		values[i] = percentage;
	}
}
