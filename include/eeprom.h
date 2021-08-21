#pragma once

#include <blinketto_mode_blink.h>
#include <blinketto_mode_counter.h>
#include <blinketto_mode_kitt.h>
#include <blinketto_mode_on.h>
#include <blinketto_mode_random_fade.h>
#include <xrstf_arduino_util.h>

#define LOAD_EEPROM(var)                                                                                               \
	struct eeprom::data var;                                                                                           \
	eeprom::load(&var);

#define SAVE_EEPROM(var) eeprom::save(&var);

namespace eeprom {
	const uint8_t VERSION = 0x01;

	const int FINGERPRINT_BYTES = 20;

	// data that is persisted in EEPROM
	struct data {
		char signature[4];
		uint8_t version;

		uint8_t mode;

		bool enableWebserver;
		bool enableWifi;
		char ssid[64];
		char password[64];
		char otaURL[128];
		char otaFingerprint[FINGERPRINT_BYTES];

		// expressed as a percentage
		uint8_t brightness;

		BlinkettoModeBlinkConfig modeBlinkConfig;
		BlinkettoModeCounterConfig modeCounterConfig;
		BlinkettoModeRandomFadeConfig modeRandomFadeConfig;
		BlinkettoModeKittConfig modeKittConfig;
	};

	void init();
	void setDefaults(data *data);
	void load(data *data);
	void save(data *data);
	bool updateFromJSON(data *config, const String &json, char *otaConstraintOut);
	bool hasFingerprint(const char fingerprint[FINGERPRINT_BYTES]);
} // namespace eeprom
