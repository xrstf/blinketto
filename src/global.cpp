#include "global.h"
#include "eeprom.h"

namespace global {
	eeprom::data config;
	volatile unsigned long sleepButtonPressed = 0;

	bool isSleeping = false;
	bool goToSleep = false;
	bool goWakeUp = false;
}
