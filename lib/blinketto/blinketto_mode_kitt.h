#pragma once

#include "blinketto.h"
#include "blinketto_interrupter.h"
#include <ArduinoJson.h>

struct BlinkettoModeKittConfig {
	uint8_t steps;
	uint32_t stepDelay;
};

class BlinkettoModeKitt {
public:
	BlinkettoModeKitt(BlinkettoModeKittConfig *config);
	bool canRun(Blinketto &blinketto);
	void run(Blinketto &blinketto, BlinkettoInterrupter &interrupter);

	static void setConfigDefaults(BlinkettoModeKittConfig *config);
	static void toJSON(BlinkettoModeKittConfig *config, JsonDocument &doc);
	static bool updateFromJSON(BlinkettoModeKittConfig *config, const JsonDocument &doc);
	static bool handleCLICommand(BlinkettoModeKittConfig *config, char *cmdArgs);

private:
	BlinkettoModeKittConfig *config;
};
