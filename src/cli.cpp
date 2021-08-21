#include "cli.h"
#include "config.h"
#include "eeprom.h"
#include "util.h"
#include "webserver.h"
#include "wifi.h"
#include "ota.h"
#include <blinketto_mode_blink.h>
#include <blinketto_mode_counter.h>
#include <blinketto_mode_kitt.h>
#include <blinketto_mode_on.h>
#include <blinketto_mode_random_fade.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

namespace cli {
	void setup() {
		Serial.begin(115200);
		Serial.println(""); // ensure a newline after the ESP8266 boot garbage output
		xrstf::serialPrintf("Blinketto %s says Moin!\n", BLINKETTO_VERSION);
	}

	void toggleWebserver(eeprom::data *config, bool flag) {
		config->enableWebserver = flag;
		eeprom::save(config);
	}

	void handleEnableWebserverCommand(eeprom::data *config) {
		toggleWebserver(config, true);
		Serial.println("OK: Successfully enabled webserver.");
		webserver::start(config);
	}

	void handleDisableWebserverCommand(eeprom::data *config) {
		toggleWebserver(config, false);
		Serial.println("OK: Successfully disabled webserver.");
		webserver::stop();
	}

	void toggleWifi(eeprom::data *config, bool flag) {
		config->enableWifi = flag;
		eeprom::save(config);
	}

	void handleEnableWifiCommand(eeprom::data *config) {
		toggleWifi(config, true);
		Serial.println("OK: Successfully enabled WiFi.");
		wifi::connect();
	}

	void handleDisableWifiCommand(eeprom::data *config) {
		toggleWifi(config, false);
		Serial.println("OK: Successfully disabled WiFi.");
		wifi::disconnect();
	}

	void handleSetWifiSSIDCommand(eeprom::data *config, char *flags) {
		if (strlen(flags) == 0) {
			Serial.println("Error: SSID must not be empty.");
			return;
		}

		strncpy(config->ssid, flags, sizeof(config->ssid));
		eeprom::save(config);

		xrstf::serialPrintf("OK: SSID successfully updated to '%s'.\n", flags);
	}

	void handleSetWifiPasswordCommand(eeprom::data *config, char *flags) {
		strncpy(config->password, flags, sizeof(config->password));
		eeprom::save(config);

		Serial.println("OK: Password successfully updated.");
	}

	void handleSetModeCommand(eeprom::data *config, char *cmdArgs) {
		uint8_t mode = modeConst(cmdArgs);
		if (mode == 0x0) {
			xrstf::serialPrintf("Error: unknown mode, try one of: %s, %s, %s, %s, %s\n", modeName(MODE_FULL_ON), modeName(MODE_BLINK), modeName(MODE_COUNTER), modeName(MODE_RANDOM_FADE), modeName(MODE_KITT));
			return;
		}

		config->mode = mode;
		eeprom::save(config);

		xrstf::serialPrintf("OK: Successfully set mode to %s.\n", modeName(mode));
	}

	void handleSetModeConfigCommand(eeprom::data *config, char *cmdArgs) {
		bool changes = false;

		if (xrstf::startsWith(cmdArgs, "blink ")) {
			changes = BlinkettoModeBlink::handleCLICommand(&config->modeBlinkConfig, cmdArgs + strlen("blink "));
		} else if (xrstf::startsWith(cmdArgs, "counter ")) {
			changes = BlinkettoModeCounter::handleCLICommand(&config->modeCounterConfig, cmdArgs + strlen("counter "));
		} else if (xrstf::startsWith(cmdArgs, "randomFade ")) {
			changes = BlinkettoModeRandomFade::handleCLICommand(&config->modeRandomFadeConfig, cmdArgs + strlen("random"
			                                                                                                    "Fade"
			                                                                                                    " "));
		} else if (xrstf::startsWith(cmdArgs, "kitt ")) {
			changes = BlinkettoModeKitt::handleCLICommand(&config->modeKittConfig, cmdArgs + strlen("kitt "));
		}

		if (changes) {
			eeprom::save(config);
			Serial.println("OK: Successfully updated configuration.");
		}
	}

	void handleSetBrightnessCommand(eeprom::data *config, char *cmdArgs) {
		if (strlen(cmdArgs) == 0) {
			Serial.println("Error: A number must be provided.");
			return;
		}

		if (strlen(cmdArgs) > 3) {
			Serial.println("Error: Number of minutes must be at most 3 digits long.");
			return;
		}

		int brightness = 0;
		if (!xrstf::safeStringToInt(cmdArgs, &brightness)) {
			Serial.println("Error: Invalid number given.");
			return;
		}

		if (brightness > 100) {
			brightness = 100;
		} else if (brightness < 0) {
			brightness = 0;
		}

		config->brightness = brightness;
		eeprom::save(config);

		xrstf::serialPrintf("OK: Successfully set brightness to %d%%.\n", brightness);
	}

	void handleWipeCommand(eeprom::data *config) {
		eeprom::setDefaults(config);
		eeprom::save(config);

		Serial.println("OK: All settings reset to factory defaults.");
	}

	void handleSetOTAURLCommand(eeprom::data *config, char *flags) {
		if (strlen(flags) == 0) {
			Serial.println("Error: URL must not be empty.");
			return;
		}

		if (strlen(flags) > 127) {
			Serial.println("Error: URL must not exceed 127 characters.");
			return;
		}

		strncpy(config->otaURL, flags, sizeof(config->otaURL));
		eeprom::save(config);

		xrstf::serialPrintf("OK: OTA URL successfully updated to '%s'.\n", flags);
	}

	void handleSetOTAFingerprintCommand(eeprom::data *config, char *flags) {
		if (strlen(flags) == 0) {
			Serial.println("Error: Fingerprint must not be empty.");
			return;
		}

		char fingerprint[eeprom::FINGERPRINT_BYTES] = {0};

		if (strlen(flags) >= eeprom::FINGERPRINT_BYTES * 2) {
			if (!xrstf::normalizeFingerprint(flags, eeprom::FINGERPRINT_BYTES, fingerprint)) {
				Serial.println("Error: Fingerprint must be a 40 digit hex string.");
				return;
			}
		}

		if (eeprom::hasFingerprint(fingerprint)) {
			memcpy(config->otaFingerprint, fingerprint, eeprom::FINGERPRINT_BYTES);

			char buf[41] = {0};
			xrstf::hexencode(fingerprint, eeprom::FINGERPRINT_BYTES, buf);
			xrstf::serialPrintf("OK: Fingerprint successfully updated to '%s'.\n", buf);
		} else {
			memset(config->otaFingerprint, 0x0, eeprom::FINGERPRINT_BYTES);
			Serial.println("OK: Fingerprint successfully removed.");
		}

		return;
	}

	void handleShowConfigurationCommand(eeprom::data *config) {
		xrstf::serialPrintf("Software version : %s\n", BLINKETTO_VERSION);
		xrstf::serialPrintf("Current mode.....: %s\n", modeName(config->mode));
		xrstf::serialPrintf("Webserver enabled: %s\n", config->enableWebserver ? "yes" : "no");
		xrstf::serialPrintf("WiFi enabled.....: %s\n", config->enableWifi ? "yes" : "no");
		xrstf::serialPrintf("WiFi SSID........: '%s'\n", config->ssid);

		char *asterisks = (char *)malloc(strlen(config->password) + 1);
		memset(asterisks, 0, strlen(config->password) + 1);
		memset(asterisks, '*', strlen(config->password));

		xrstf::serialPrintf("WiFi password....: '%s'\n", asterisks);
		delete asterisks;

		char buf[41] = {0};
		if (eeprom::hasFingerprint(config->otaFingerprint)) {
			xrstf::hexencode(config->otaFingerprint, eeprom::FINGERPRINT_BYTES, buf);
		} else {
			strncpy(buf, "(none)", sizeof(config->otaFingerprint));
		}

		xrstf::serialPrintf("OTA URL..........: %s\n", config->otaURL);
		xrstf::serialPrintf("OTA Fingerprint..: %s\n", buf);

		if (wifi::connected) {
			xrstf::serialPrintf("Current IP.......: %s\n", wifi::localIP.toString().c_str());
		}
	}

	void handleLocalIPCommand() {
		if (wifi::connected) {
			xrstf::serialPrintf("My IP is %s.\n", wifi::localIP.toString().c_str());
		} else {
			Serial.println("Not connected.");
		}
	}

	void handleOTACommand(eeprom::data *config, char *flags) {
		if (strlen(config->otaURL) == 0) {
			Serial.println("Error: No OTA URL configured.");
			return;
		}

		String url(config->otaURL);
		String constraint("latest");

		if (flags && strlen(flags) > 0) {
			constraint = flags;
		}

		ota::update(url, constraint, config->otaFingerprint);
	}

	void clear() {
		while (Serial.available() > 0) {
			Serial.read();
		}
	}

	void handleCommand(eeprom::data *config) {
		if (Serial.available() > 0) {
			char commandStr[128];
			Serial.readBytesUntil('\n', commandStr, 127);

			// trim trailing newlines, thanks to https://stackoverflow.com/a/28462221
			commandStr[strcspn(commandStr, "\r\n")] = 0;

			if (xrstf::startsWith(commandStr, "set-wifi-ssid ")) {
				handleSetWifiSSIDCommand(config, commandStr + strlen("set-wifi-ssid "));
			} else if (xrstf::startsWith(commandStr, "set-wifi-password ")) {
				handleSetWifiPasswordCommand(config, commandStr + strlen("set-wifi-password "));
			} else if (xrstf::startsWith(commandStr, "set-mode ")) {
				handleSetModeCommand(config, commandStr + strlen("set-mode "));
			} else if (xrstf::startsWith(commandStr, "set-brightness ")) {
				handleSetBrightnessCommand(config, commandStr + strlen("set-brightness "));
			} else if (xrstf::startsWith(commandStr, "set-mode-config ")) {
				handleSetModeConfigCommand(config, commandStr + strlen("set-mode-config "));
			} else if (xrstf::startsWith(commandStr, "set-ota-url ")) {
				handleSetOTAURLCommand(config, commandStr + strlen("set-ota-url "));
			} else if (xrstf::startsWith(commandStr, "set-ota-fingerprint ")) {
				handleSetOTAFingerprintCommand(config, commandStr + strlen("set-ota-fingerprint "));
			} else if (strcmp(commandStr, "enable-webserver") == 0) {
				handleEnableWebserverCommand(config);
			} else if (strcmp(commandStr, "disable-webserver") == 0) {
				handleDisableWebserverCommand(config);
			} else if (strcmp(commandStr, "enable-wifi") == 0) {
				handleEnableWifiCommand(config);
			} else if (strcmp(commandStr, "disable-wifi") == 0) {
				handleDisableWifiCommand(config);
			} else if (strcmp(commandStr, "local-ip") == 0) {
				handleLocalIPCommand();
			} else if (strcmp(commandStr, "wipe") == 0) {
				handleWipeCommand(config);
			} else if (strcmp(commandStr, "show-configuration") == 0) {
				handleShowConfigurationCommand(config);
			} else if (strcmp(commandStr, "ota") == 0) {
				handleOTACommand(config, NULL);
			} else if (xrstf::startsWith(commandStr, "ota ")) {
				handleOTACommand(config, commandStr + strlen("ota "));
			} else {
				xrstf::serialPrintf("Error: unknown command '%s'.\n", commandStr);
			}

			clear();
		}
	}
} // namespace cli
