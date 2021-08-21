#pragma once

#include "driver.h"
#include "eeprom.h"
#include "plugman.h"
#include <blinketto.h>
#include <blinketto_interrupter.h>

class Interrupter : public BlinkettoInterrupter {
public:
	Interrupter(struct eeprom::data *config, PlugManager &plugMan, PCA9624Driver &ledDriver);
	bool shouldInterrupt(Blinketto &blinketto);

private:
	eeprom::data *config;
	PlugManager &plugMan;
	PCA9624Driver &ledDriver;
	bool sleeping;
	unsigned long lastButtonPress;

	void toggleSleep(Blinketto &blinketto, bool sleeping);
};
