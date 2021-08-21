#pragma once

#include "eeprom.h"

namespace global {
	extern eeprom::data config;

	// set to the curren time whenever the button is pressed
	extern volatile unsigned long sleepButtonPressed;

	// set by the HTTP webserver
	extern bool goToSleep;
	extern bool goWakeUp;
}
