#pragma once

#include "eeprom.h"

namespace webserver {
	extern bool running;

	void setup();
	void start(eeprom::data *config);
	void stop();
	void handleRequests(eeprom::data *config);
} // namespace webserver
