#pragma once

#include "blinketto.h"
#include "blinketto_interrupter.h"
#include <ArduinoJson.h>

struct BlinkettoModeCounterConfig {
	uint8_t steps;
	uint32_t stepDelay;
	uint32_t timeBetweenNumbers;
	uint32_t timeBetweenEndings;
};

class BlinkettoModeCounter {
public:
	BlinkettoModeCounter(BlinkettoModeCounterConfig *config);
	bool canRun(Blinketto &blinketto);
	void run(Blinketto &blinketto, BlinkettoInterrupter &interrupter);

	static void setConfigDefaults(BlinkettoModeCounterConfig *config);
	static bool updateFromJSON(BlinkettoModeCounterConfig *config, const JsonDocument &doc);
	static bool handleCLICommand(BlinkettoModeCounterConfig *config, char *cmdArgs);

private:
	BlinkettoModeCounterConfig *config;
};
