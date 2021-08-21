#pragma once

#include "eeprom.h"

namespace cli {
	void setup();
	void handleCommand(eeprom::data *config);
} // namespace cli
