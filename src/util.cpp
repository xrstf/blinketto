#include "util.h"
#include "config.h"
#include <Arduino.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

const char *modeName(uint8_t mode) {
	switch (mode) {
	case MODE_FULL_ON:
		return "fullOn";
	case MODE_BLINK:
		return "blink";
	case MODE_COUNTER:
		return "counter";
	case MODE_RANDOM_FADE:
		return "randomFade";
	case MODE_KITT:
		return "kitt";
	default:
		return "none";
	}
}

uint8_t modeConst(const char *name) {
	if (xrstf::stricmp(name, modeName(MODE_FULL_ON)) == 0) {
		return MODE_FULL_ON;
	}
	if (xrstf::stricmp(name, modeName(MODE_BLINK)) == 0) {
		return MODE_BLINK;
	}
	if (xrstf::stricmp(name, modeName(MODE_COUNTER)) == 0) {
		return MODE_COUNTER;
	}
	if (xrstf::stricmp(name, modeName(MODE_RANDOM_FADE)) == 0) {
		return MODE_RANDOM_FADE;
	}
	if (xrstf::stricmp(name, modeName(MODE_KITT)) == 0) {
		return MODE_KITT;
	}

	return MODE_NONE;
}

void blink(unsigned char times, int duration) {
	for (unsigned char i = 0; i < times; i++) {
		digitalWrite(LED_PIN, HIGH);
		delay(duration);
		digitalWrite(LED_PIN, LOW);
		delay(duration);
	}

	delay(500);
}

void setupLED() {
	if (LED_PIN > 0) {
		pinMode(LED_PIN, OUTPUT);
		enableLED();
	}
}

void setLED(bool enabled) {
	if (LED_PIN > 0) {
		digitalWrite(LED_PIN, enabled ? HIGH : LOW);
	}
}

void enableLED() {
	setLED(true);
}

void disableLED() {
	setLED(false);
}

bool startsWith(const char *a, const char *b) {
	if (strncmp(a, b, strlen(b)) == 0)
		return 1;
	return 0;
}
