/*
 To flash this, choose the following settings in the Arduino IDE:

     - board:        Generic ESP8266 Board
     - upload speed: 115200
     - reset mode:   "dtr (aka nodemcu)" for mini-reset-circuit
                                     "no dtr (aka ck)" for kevin's circuit

 To ensure EEPROM works:

     - flash frequency: 26Mhz
     - flash size:      2MB (FS: 64KB)

 The remaining things can be left as-is.
*/

#include "cli.h"
#include "config.h"
#include "driver.h"
#include "eeprom.h"
#include "global.h"
#include "interrupter.h"
#include "plugman.h"
#include "webserver.h"
#include "wifi.h"
#include <blinketto.h>
#include <blinketto_mode_blink.h>
#include <blinketto_mode_counter.h>
#include <blinketto_mode_kitt.h>
#include <blinketto_mode_on.h>
#include <blinketto_mode_random_fade.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

PlugManager plugMan(SHIFTREGISTER_CLK_PIN, SHIFTREGISTER_LOAD_PIN, SHIFTREGISTER_DATA_PIN);
PCA9624Driver *driver;
Blinketto *blinketto;
Interrupter *interrupter;

bool runModeOn() {
	BlinkettoModeOn mode;
	if (!mode.canRun(*blinketto)) {
		return false;
	}

	mode.run(*blinketto, *interrupter);
	return true;
}

bool runModeBlink() {
	BlinkettoModeBlink mode(&global::config.modeBlinkConfig);
	if (!mode.canRun(*blinketto)) {
		return false;
	}

	mode.run(*blinketto, *interrupter);
	return true;
}

bool runModeCounter() {
	BlinkettoModeCounter mode(&global::config.modeCounterConfig);
	if (!mode.canRun(*blinketto)) {
		return false;
	}

	mode.run(*blinketto, *interrupter);
	return true;
}

bool runModeRandomFade() {
	BlinkettoModeRandomFade mode(&global::config.modeRandomFadeConfig);
	if (!mode.canRun(*blinketto)) {
		return false;
	}

	mode.run(*blinketto, *interrupter);
	return true;
}

bool runModeKitt() {
	BlinkettoModeKitt mode(&global::config.modeKittConfig);
	if (!mode.canRun(*blinketto)) {
		return false;
	}

	mode.run(*blinketto, *interrupter);
	return true;
}

// without IRAM_ATTR, the ESP crashes when calling attachInterrupt,
// see https://github.com/esp8266/Arduino/issues/6237
void IRAM_ATTR onSleepButtonPress() {
	global::sleepButtonPressed = millis();
}

void setup() {
	xrstf::setupLED(LED_PIN);
	cli::setup();
	eeprom::init();
	eeprom::load(&global::config);
	wifi::connect();
	webserver::setup();
	webserver::start(&global::config);

	pinMode(SLEEP_BUTTON_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SLEEP_BUTTON_PIN), onSleepButtonPress, FALLING);

	Wire.begin(I2C_SDA_PIN, I2C_SCK_PIN);

	driver = new PCA9624Driver(I2C_ADDRESS);
	driver->setLEDStates(plugMan.getStatus());

	xrstf::blinkLED(LED_PIN, 3);

	delay(1000);

	blinketto   = new Blinketto(*driver, 0xFF);
	interrupter = new Interrupter(&global::config, plugMan, *driver);

	// set the actual full brightness
	// (Blinketto constructor requires 0..0xFF, but EEPROM stores a percentage)
	blinketto->setFullBrightness(global::config.brightness);
}

void loop() {
	// Loop until we have at least a single output plugged in
	if (blinketto->numActiveLEDs() == 0) {
		interrupter->shouldInterrupt(*blinketto);
		delay(250);
		return;
	}

	// if a runMode* function returns false, that means that
	// the mode is not possible to be used (e.g. because of too
	// few outputs); in those cases we simply skip over to the
	// next mode.

	switch (global::config.mode) {
	case MODE_BLINK:
		if (runModeBlink()) {
			break;
		}
		// fallthroughh

	case MODE_COUNTER:
		if (runModeCounter()) {
			break;
		}
		// fallthroughh

	case MODE_RANDOM_FADE:
		if (runModeRandomFade()) {
			break;
		}
		// fallthroughh

	case MODE_KITT:
		if (runModeKitt()) {
			break;
		}
		// fallthroughh

	case MODE_FULL_ON:
		if (runModeOn()) {
			break;
		}
		// fallthroughh

	case MODE_NONE:
		delay(250);
		break;
	}
}
