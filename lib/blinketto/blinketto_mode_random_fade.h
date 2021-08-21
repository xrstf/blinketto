#pragma once

#include "blinketto.h"
#include "blinketto_interrupter.h"
#include <ArduinoJson.h>

struct BlinkettoModeRandomFadeConfig {
	uint8_t steps;
	uint32_t stepDelay;
	uint8_t minChangePerInterval;
	uint8_t maxChangePerInterval;
};

class BlinkettoModeRandomFade {
public:
	BlinkettoModeRandomFade(BlinkettoModeRandomFadeConfig *config);
	bool canRun(Blinketto &blinketto);
	void run(Blinketto &blinketto, BlinkettoInterrupter &interrupter);

	static void setConfigDefaults(BlinkettoModeRandomFadeConfig *config);
	static bool updateFromJSON(BlinkettoModeRandomFadeConfig *config, const JsonDocument &doc);
	static bool handleCLICommand(BlinkettoModeRandomFadeConfig *config, char *cmdArgs);

private:
	BlinkettoModeRandomFadeConfig *config;

	void calculateNewValues(uint8_t *values, uint8_t numValues);
};
