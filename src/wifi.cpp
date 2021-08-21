#include "global.h"
#include "util.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <string.h>
#include <xrstf_arduino_util.h>
#include <xrstf_util.h>

namespace wifi {
	bool connected;
	IPAddress localIP;

	void connect() {
		if (global::config.enableWifi && strlen(global::config.ssid) > 0 && strlen(global::config.password) > 0) {
			xrstf::serialPrintf("Connecting to Wifi '%s'...\n", global::config.ssid);

			WiFi.setHostname("blinketto");
			WiFi.setAutoReconnect(true);
			WiFi.begin(global::config.ssid, global::config.password);

			int attempts = 60; // 60 times 500ms = 30s timeout
			while (attempts > 0 && WiFi.status() != WL_CONNECTED) {
				delay(500);
				attempts--;
			}

			if (WiFi.status() != WL_CONNECTED) {
				Serial.println("Connection failed :(");
				return;
			}

			localIP = WiFi.localIP();

			xrstf::serialPrintf("Connection established, my IP is %s!\n", localIP.toString().c_str());
			connected = true;
		}
	}

	void disconnect() {
		if (connected) {
			WiFi.disconnect(true);
			Serial.println("Wifi disconnected.");
			connected = false;
		}
	}
} // namespace wifi
