#include "blinketto_mode_blink.h"
#include "blinketto_util.h"
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

BlinkettoModeBlink::BlinkettoModeBlink(BlinkettoModeBlinkConfig *config) : config(config) {
}

void BlinkettoModeBlink::setConfigDefaults(BlinkettoModeBlinkConfig *config) {
	config->steps            = 20;
	config->stepDelay        = 50;
	config->timeBetweenFades = 1000;
}

bool BlinkettoModeBlink::canRun(Blinketto &blinketto) {
	return true;
}

bool BlinkettoModeBlink::updateFromJSON(BlinkettoModeBlinkConfig *config, const JsonDocument &doc) {
	bool changes = false;

	if (doc.containsKey("steps")) {
		int steps = doc["steps"];
		if (steps >= 0 && steps <= 60 && ((char)steps) != config->steps) {
			xrstf::serialPrintf("Update: Setting modeBlink.steps to %d.\n", steps);
			config->steps = (uint8_t)steps;
			changes       = true;
		}
	}

	if (doc.containsKey("stepDelay")) {
		int stepDelay = doc["stepDelay"];
		if (stepDelay >= 0 && stepDelay <= 60 && ((char)stepDelay) != config->stepDelay) {
			xrstf::serialPrintf("Update: Setting modeBlink.stepDelay to %d.\n", stepDelay);
			config->stepDelay = (uint32_t)stepDelay;
			changes           = true;
		}
	}

	if (doc.containsKey("timeBetweenFades")) {
		int timeBetweenFades = doc["timeBetweenFades"];
		if (timeBetweenFades >= 0 && timeBetweenFades <= 60 && ((char)timeBetweenFades) != config->timeBetweenFades) {
			xrstf::serialPrintf("Update: Setting modeBlink.timeBetweenFades to %d.\n", timeBetweenFades);
			config->timeBetweenFades = (uint32_t)timeBetweenFades;
			changes                  = true;
		}
	}

	return changes;
}

bool BlinkettoModeBlink::handleCLICommand(BlinkettoModeBlinkConfig *config, char *cmdArgs) {
	if (xrstf::startsWith(cmdArgs, "steps ")) {
		return setUint8ConfigValue(&config->steps, cmdArgs + strlen("steps "), 1, 50);
	}

	if (xrstf::startsWith(cmdArgs, "stepDelay ")) {
		return setUint32ConfigValue(&config->stepDelay, cmdArgs + strlen("stepDelay "), 0, 100);
	}

	if (xrstf::startsWith(cmdArgs, "timeBetweenFades ")) {
		return setUint32ConfigValue(&config->timeBetweenFades, cmdArgs + strlen("timeBetweenFades "), 0, 10000);
	}

	xrstf::serialPrintf("Error: Unknown sub command '%s'.\n", cmdArgs);
	return false;
}

void BlinkettoModeBlink::run(Blinketto &blinketto, BlinkettoInterrupter &interrupter) {
	blinketto.transitionToBitmap(0xFF, this->config->steps, 100, this->config->stepDelay);
	STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)

	delay(this->config->timeBetweenFades);
	STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)

	blinketto.transitionToBitmap(0xFF, this->config->steps, 0, this->config->stepDelay);
	STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)

	delay(this->config->timeBetweenFades);
	STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
}
