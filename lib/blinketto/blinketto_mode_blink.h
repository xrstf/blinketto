#pragma once

#include "blinketto.h"
#include "blinketto_interrupter.h"
#include <ArduinoJson.h>

struct BlinkettoModeBlinkConfig {
	uint8_t steps;
	uint32_t stepDelay;
	uint32_t timeBetweenFades;
};

class BlinkettoModeBlink {
public:
	BlinkettoModeBlink(BlinkettoModeBlinkConfig *config);
	bool canRun(Blinketto &blinketto);
	void run(Blinketto &blinketto, BlinkettoInterrupter &interrupter);

	static void setConfigDefaults(BlinkettoModeBlinkConfig *config);
	static void toJSON(BlinkettoModeBlinkConfig *config, JsonDocument &doc);
	static bool updateFromJSON(BlinkettoModeBlinkConfig *config, const JsonDocument &doc);
	static bool handleCLICommand(BlinkettoModeBlinkConfig *config, char *cmdArgs);

private:
	BlinkettoModeBlinkConfig *config;
};
