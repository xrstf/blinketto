#include "eeprom.h"
#include "config.h"
#include "util.h"
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

namespace eeprom {
	const char signature[] = {'B', 'L', 'K', 'O'};

	struct signatureOnlyData {
		char signature[4];
		char version;
	};

	/**
	 * migrate and/or set defaults
	 */
	void init() {
		signatureOnlyData sig;
		EEPROM.begin(sizeof(signatureOnlyData));
		EEPROM.get(0, sig);
		EEPROM.end();

		// no signature present yet, init current defaults
		if (strncmp(sig.signature, signature, sizeof(sig.signature)) != 0) {
			data d;
			setDefaults(&d);
			save(&d);

			return;
		}

		// if (sig.version == N) {
		// 	Serial.println("Migrating version N to N+1...");
		// 	migrateVNtoVN+1();
		// }
	}

	void setDefaults(data *d) {
		d->version         = VERSION;
		d->mode            = MODE_FULL_ON;
		d->enableWebserver = true;
		d->enableWifi      = false;
		d->brightness      = 100; // percent

		strncpy(d->signature, signature, sizeof(d->signature));
		memset(d->ssid, 0x0, sizeof(d->ssid));
		memset(d->password, 0x0, sizeof(d->password));
		memset(d->otaURL, 0x0, sizeof(d->otaURL));
		memset(d->otaFingerprint, 0x0, sizeof(d->otaFingerprint));

		BlinkettoModeBlink::setConfigDefaults(&d->modeBlinkConfig);
		BlinkettoModeCounter::setConfigDefaults(&d->modeCounterConfig);
		BlinkettoModeRandomFade::setConfigDefaults(&d->modeRandomFadeConfig);
		BlinkettoModeKitt::setConfigDefaults(&d->modeKittConfig);
	}

	void load(data *d) {
		EEPROM.begin(sizeof(data));
		EEPROM.get(0, *d);
		EEPROM.end();
	}

	void save(data *d) {
		EEPROM.begin(sizeof(data));
		EEPROM.put(0, *d);
		EEPROM.end();
	}

	bool updateFromJSON(data *config, const String &json, char *otaConstraintOut) {
		if (json.length() == 0) {
			return true;
		}

		StaticJsonDocument<256> doc;

		DeserializationError error = deserializeJson(doc, json.c_str(), json.length());
		if (error) {
			xrstf::serialPrintf("Error: Failed to parse JSON: %s\n", error.c_str());
			return false;
		}

		// Only set this when fields _actually_ change, not only
		// if they are present in the server's response. Otherwise
		// a lazy server might force us to constantly update EEPROM.
		bool changes = false;

		if (doc.containsKey("mode")) {
			const char *modeStr = doc["mode"];
			uint8_t mode        = modeConst(modeStr);
			if (mode != MODE_NONE && mode != config->mode) {
				xrstf::serialPrintf("Update: Setting mode to '%s'.\n", modeStr);
				config->mode = mode;
				changes      = true;
			}
		}

		if (doc.containsKey("brightness")) {
			uint8_t brightness = doc["brightness"];
			if (brightness > 0 && brightness <= 100 && brightness != config->brightness) {
				xrstf::serialPrintf("Update: Setting brightness to %d%%.\n", brightness);
				config->brightness = brightness;
				changes            = true;
			}
		}

		if (doc.containsKey("ssid")) {
			const char *ssid = doc["ssid"];
			if (strcmp(ssid, config->ssid) != 0) {
				xrstf::serialPrintf("Update: Setting WiFi SSID to '%s'.\n", ssid);
				strncpy(config->ssid, ssid, sizeof(config->ssid));
				changes = true;
			}
		}

		if (doc.containsKey("password")) {
			const char *password = doc["password"];
			if (strcmp(password, config->password) != 0) {
				Serial.println("Update: Updated WiFi password.");
				strncpy(config->password, password, sizeof(config->password));
				changes = true;
			}
		}

		if (doc.containsKey("otaURL")) {
			const char *otaURL = doc["otaURL"];
			if (strcmp(otaURL, config->otaURL) != 0) {
				xrstf::serialPrintf("Update: Setting otaURL to '%s'.\n", otaURL);
				strncpy(config->otaURL, otaURL, sizeof(config->otaURL));
				changes = true;
			}
		}

		if (doc.containsKey("otaFingerprint")) {
			char fingerprint[eeprom::FINGERPRINT_BYTES] = {0};

			const char *otaFingerprint = doc["otaFingerprint"];
			if (xrstf::normalizeFingerprint(otaFingerprint, eeprom::FINGERPRINT_BYTES, fingerprint)) {
				if (memcmp(config->otaFingerprint, fingerprint, eeprom::FINGERPRINT_BYTES) != 0) {
					Serial.println("Update: Updating OTA fingerprint.");
					memcpy(config->otaFingerprint, fingerprint, sizeof(config->otaFingerprint));
					changes = true;
				}
			}
		}

		if (doc.containsKey("modeBlink")) {
			StaticJsonDocument<96> blinkConfig = doc["modeBlink"];
			if (BlinkettoModeBlink::updateFromJSON(&config->modeBlinkConfig, blinkConfig)) {
				changes = true;
			}
		}

		if (doc.containsKey("modeCounter")) {
			StaticJsonDocument<96> blinkConfig = doc["modeCounter"];
			if (BlinkettoModeCounter::updateFromJSON(&config->modeCounterConfig, blinkConfig)) {
				changes = true;
			}
		}

		if (doc.containsKey("modeRandomFade")) {
			StaticJsonDocument<96> blinkConfig = doc["modeRandomFade"];
			if (BlinkettoModeRandomFade::updateFromJSON(&config->modeRandomFadeConfig, blinkConfig)) {
				changes = true;
			}
		}

		if (doc.containsKey("modeKitt")) {
			StaticJsonDocument<96> blinkConfig = doc["modeKitt"];
			if (BlinkettoModeKitt::updateFromJSON(&config->modeKittConfig, blinkConfig)) {
				changes = true;
			}
		}

		bool temporary = !doc.containsKey("temp") || ((bool) doc["temp"] == false);

		if (changes && !temporary) {
			save(config);
		}

		// copy the OTA constraint out, so that the original request handling
		// can finish before beginning the OTA process.
		if (otaConstraintOut && strlen(config->otaURL) > 0 && doc.containsKey("otaConstraint")) {
			strncpy(otaConstraintOut, doc["otaConstraint"], 32);
		}

		return true;
	}

	bool hasFingerprint(const char fingerprint[FINGERPRINT_BYTES]) {
		for (size_t i = 0; i < FINGERPRINT_BYTES; ++i) {
			if (fingerprint[i]) {
				return true;
			}
		}

		return false;
	}
} // namespace eeprom
