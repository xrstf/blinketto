#pragma once

#include <stdint.h> // uint8_t etc.
#include <stdio.h>

#define BLKO_MAX_OUTPUTS 8
#define BLKO_MAX_BRIGHTNESS 0xFF

#define BLKO_BRIGHTNESS_MAX 0xFF
#define BLKO_BRIGHTNESS_OFF 0x0

#define BLKO_DIRECTION_KEEP 0
#define BLKO_DIRECTION_UP 1
#define BLKO_DIRECTION_DOWN 2

#define BLKO_MODE_UNDEFINED 0 // just the default, do not set this actively
#define BLKO_MODE_OFF 1
#define BLKO_MODE_ON 2
#define BLKO_MODE_INDIVIDUAL 3

class BlinkettoDriver {
public:
	BlinkettoDriver(){};

	virtual void setMode(uint8_t mode) = 0;

	// set all LEDs to a certain brightness
	virtual void setAll(uint8_t brightness) = 0;

	// turn all LEDs off
	virtual void turnOff() = 0;

	// turn all LEDs on
	virtual void turnOn(uint8_t brightness = BLKO_BRIGHTNESS_MAX) = 0;

	// set the brightness of a single LED
	virtual bool setBrightness(uint8_t led, uint8_t brightness) = 0;

	// this is like setBrightness, but updates all LEDs in one operation
	virtual void setBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) = 0;

	// this informs the object about new plug-in states,
	// usually called after an interrupt happened; it
	// is someone else's responsibility to handle that
	// and present a completed bitmap
	virtual void setLEDStates(uint8_t newState) = 0;

	// returns the LED states
	virtual uint8_t getLEDStates() = 0;

	virtual void getBrightnesses(uint8_t brightnesses[BLKO_MAX_OUTPUTS]) = 0;

	// returns the number of plugged in outputs
	uint8_t numActiveLEDs();

	// transition all LEDs from the current state to a new
	// state; this assumes that all LEDs are fully on/off
	// before and after the transition.
	void transition(uint8_t targetBrightnesses[BLKO_MAX_OUTPUTS], uint8_t steps, uint32_t stepDelay = 50);
};

class DummyBlinkettoDriver : public BlinkettoDriver {
public:
	DummyBlinkettoDriver();

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
	// bitmap detailing what output jack have plugs
	// plugged in to them
	uint8_t ledStates;

	// current state of each of the outputs
	uint8_t brightnesses[BLKO_MAX_OUTPUTS];

	uint8_t mode;

	void update();
};

uint8_t mapPercentageToMax(uint8_t percentage, uint8_t max);

/**
 * Blinketto exposes addressing based on logical
 * LED positions, i.e. if there are 5 devices plugged in,
 * the possible indices are [0..4]. The class translates
 * this to the actually plugged in outputs on the hardware.
 *
 * This class also translates a brightness percentage into
 * a configurable maximum brightness value, because LEDs often
 * do not look much brighter at 100% than they look at 90%.
 * This would make some transitions looks weird, so the max
 * brightness can be tweaked.
 */
class Blinketto {
public:
	Blinketto(BlinkettoDriver &hardware, uint8_t fullBrightness = BLKO_BRIGHTNESS_MAX);

	// set all LEDs to a certain brightness
	void setAll(uint8_t brightnessPercent);

	// turn all LEDs off
	void turnOff();

	// turn all LEDs on
	void turnOn(uint8_t brightnessPercent = 100);

	// set the brightness of a single LED
	bool setBrightness(uint8_t led, uint8_t brightnessPercent);

	// this is like setBrightness, but updates all LEDs in one operation;
	// the given array of brightnesses MUST have exactly the same amount
	// of elements as numActiveLEDs().
	void setBrightnesses(uint8_t brightnessPercentages[]);

	void transition(uint8_t brightnessPercentages[], uint8_t steps, uint32_t stepDelay = 50);
	void transitionToBitmap(uint8_t bitmap, uint8_t steps, uint8_t brightnessPercent, uint32_t stepDelay = 50);

	// update actual LED state in the real world
	// (i.e. tell LED driver chip to udpate)
	void update();

	// returns the number of plugged in outputs
	uint8_t numActiveLEDs();

	void getBrightnesses(uint8_t brightnesses[]);

	void mapPercentages(uint8_t brightnessPercentages[], uint8_t result[]);

	void setFullBrightness(uint8_t brightnessPercentage);

	// map percentage to brightness: [0..100] ==> [0..fullBrightness]
	uint8_t mapPercentage(uint8_t percentage);

	// map brightness to percentage: [0..fullBrightness] ==> [0..100]
	uint8_t mapBrightness(uint8_t brightness);

private:
	BlinkettoDriver &driver;
	uint8_t fullBrightness;
};
