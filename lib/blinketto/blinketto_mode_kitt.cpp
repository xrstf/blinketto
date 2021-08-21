#include "blinketto_mode_kitt.h"
#include "blinketto_util.h"
#include <stdint.h> // uint8_t etc.
#include <stdio.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

BlinkettoModeKitt::BlinkettoModeKitt(BlinkettoModeKittConfig *config) : config(config) {
}

bool BlinkettoModeKitt::canRun(Blinketto &blinketto) {
	return blinketto.numActiveLEDs() == 8;
}

void BlinkettoModeKitt::setConfigDefaults(BlinkettoModeKittConfig *config) {
	config->steps     = 1;
	config->stepDelay = 0;
}

bool BlinkettoModeKitt::updateFromJSON(BlinkettoModeKittConfig *config, const JsonDocument &doc) {
	bool changes = false;

	if (doc.containsKey("steps")) {
		int steps = doc["steps"];
		if (steps >= 0 && steps <= 60 && ((char)steps) != config->steps) {
			xrstf::serialPrintf("Update: Setting modeKitt.steps to %d.\n", steps);
			config->steps = (uint8_t)steps;
			changes       = true;
		}
	}

	if (doc.containsKey("stepDelay")) {
		int stepDelay = doc["stepDelay"];
		if (stepDelay >= 0 && stepDelay <= 60 && ((char)stepDelay) != config->stepDelay) {
			xrstf::serialPrintf("Update: Setting modeKitt.stepDelay to %d.\n", stepDelay);
			config->stepDelay = (uint32_t)stepDelay;
			changes           = true;
		}
	}

	return changes;
}

bool BlinkettoModeKitt::handleCLICommand(BlinkettoModeKittConfig *config, char *cmdArgs) {
	if (xrstf::startsWith(cmdArgs, "steps ")) {
		return setUint8ConfigValue(&config->steps, cmdArgs + strlen("steps "), 1, 50);
	}

	if (xrstf::startsWith(cmdArgs, "stepDelay ")) {
		return setUint32ConfigValue(&config->stepDelay, cmdArgs + strlen("stepDelay "), 0, 100);
	}

	xrstf::serialPrintf("Error: Unknown sub command '%s'.\n", cmdArgs);
	return false;
}

// 8 LED brightness percentages times 28 frames
uint8_t kittData[] = {
																																																		0x64,
																																																		0x64,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x4B,
																																																		0x64,
																																																		0x32,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x4B,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x32,
																																																		0x64,
																																																		0x64,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x32,
																																																		0x64,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x4B,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x32,
																																																		0x64,
																																																		0x64,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x32,
																																																		0x64,
																																																		0x4B,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x4B,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x64,
																																																		0x64,
																																																		0x32,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x64,
																																																		0x32,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x19,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
																																																		0x64,
																																																		0x64,
																																																		0x4B,
																																																		0x32,
																																																		0x19,
																																																		0x00,
																																																		0x00,
																																																		0x00,
};

void BlinkettoModeKitt::run(Blinketto &blinketto, BlinkettoInterrupter &interrupter) {
	while (true) {
		for (uint8_t frame = 0; frame < 28; ++frame) {
			blinketto.transition(&kittData[frame * 8], this->config->steps, this->config->stepDelay);
			STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
			yield();
		}
	}
}
