#pragma once

#include <PCA9624.h>
#include <blinketto.h>

// denotes when the PCA chip has just booted up
// and we're not set sure what mode is set
#define BLINKETTO_NO_MODE 0xFF

class PCA9624Driver : public BlinkettoDriver {
public:
	PCA9624Driver(uint8_t i2cAddress);

	void setMode(uint8_t mode);
	void setAll(uint8_t brightness);
	void turnOff();
	void turnOn(uint8_t brightness = BLKO_BRIGHTNESS_MAX);
	bool setBrightness(uint8_t led, uint8_t brightness);
	void setBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]);
	void setLEDStates(uint8_t newState);
	uint8_t getLEDStates();
	void getBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]);

private:
	PCA9624 driver;

	// just a cache so we don't needlessly override the
	// mode every time; this works well because we do
	// not allow the user to set arbitrary modes per LED
	// in this class... hehehehe... let's hope it doesn't
	// bit us in the behind.
	uint8_t mode;

	// bitmap detailing what output jack have plugs
	// plugged in to them
	uint8_t ledStates;

	// current state of each of the outputs
	uint8_t brightnesses[BLKO_MAX_OUTPUTS];

	// this is raw access to the hardware device
	void writeLedOutputState(uint8_t active, uint8_t inactive = PCA9624_LED_OFF);
};
