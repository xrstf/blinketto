#include "interrupter.h"
#include "cli.h"
#include "global.h"
#include "webserver.h"
#include <Arduino.h>
#include <xrstf_util.h>
#include <xrstf_arduino_util.h>

Interrupter::Interrupter(eeprom::data *config, PlugManager &plugMan, PCA9624Driver &ledDriver)
	: config(config), plugMan(plugMan), ledDriver(ledDriver), sleeping(false), lastButtonPress(0) {
}

bool Interrupter::shouldInterrupt(Blinketto &blinketto) {
	// update plug status
	uint8_t oldStatus = this->ledDriver.getLEDStates();
	uint8_t newStatus = this->plugMan.getStatus();

	if (oldStatus != newStatus) {
		this->ledDriver.setLEDStates(newStatus); // this resets all outputs (i.e. turns them off)
	}

	uint8_t prevBrightness = this->config->brightness;

	// check if the sleep button was pressed
	noInterrupts();
	if (global::sleepButtonPressed > 0) {
		if (global::sleepButtonPressed - this->lastButtonPress > 500) {
			this->lastButtonPress = global::sleepButtonPressed;
			this->toggleSleep(blinketto, !this->sleeping);
		}

		global::sleepButtonPressed = 0;
	}
	interrupts();

	uint8_t currentMode = this->config->mode;

	cli::handleCommand(this->config);
	webserver::handleRequests(this->config);

	// if the user changed the brightness, that means we want
	// to "break out" of the sleep mode (e.g. via a HTTP call)
	if (this->config->brightness != prevBrightness) {
		blinketto.setFullBrightness(this->config->brightness);
		this->sleeping = false;
	}

	// allow the user to toggle sleep mode via the webserver
	if (global::goToSleep) {
		this->toggleSleep(blinketto, true);
		global::goToSleep = false;
	}

	if (global::goWakeUp) {
		this->toggleSleep(blinketto, false);
		global::goWakeUp = false;
	}

	return this->config->mode != currentMode || oldStatus != newStatus;
}

void Interrupter::toggleSleep(Blinketto &blinketto, bool sleeping) {
	this->sleeping = sleeping;
	blinketto.setFullBrightness(sleeping ? 0 : this->config->brightness);
}
