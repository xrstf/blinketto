#include "webserver.h"
#include "LittleFS.h"
#include "config.h"
#include "eeprom.h"
#include "global.h"
#include "util.h"
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

namespace webserver {
	ESP8266WebServer server(80);
	bool running;

	eeprom::data *tmpContext;

	void handleGetConfigurationRequest() {
		StaticJsonDocument<512> doc;

		doc["appVersion"] = BLINKETTO_VERSION;
		doc["mode"]       = modeName(tmpContext->mode);
		doc["brightness"] = tmpContext->brightness;
		doc["ssid"]       = tmpContext->ssid;
		doc["sleeping"]   = global::isSleeping;

		StaticJsonDocument<128> modeBlinkConfig;
		BlinkettoModeBlink::toJSON(&tmpContext->modeBlinkConfig, modeBlinkConfig);

		StaticJsonDocument<128> modeCounterConfig;
		BlinkettoModeCounter::toJSON(&tmpContext->modeCounterConfig, modeCounterConfig);

		StaticJsonDocument<128> modeKittConfig;
		BlinkettoModeKitt::toJSON(&tmpContext->modeKittConfig, modeKittConfig);

		StaticJsonDocument<128> modeRandomFadeConfig;
		BlinkettoModeRandomFade::toJSON(&tmpContext->modeRandomFadeConfig, modeRandomFadeConfig);

		doc["modeBlink"]      = modeBlinkConfig;
		doc["modeCounter"]    = modeCounterConfig;
		doc["modeKitt"]       = modeKittConfig;
		doc["modeRandomFade"] = modeRandomFadeConfig;

		char buf[512];
		memset(buf, 0, sizeof(buf));
		serializeJson(doc, buf);

		server.keepAlive(false);
		server.send(200, "application/json", buf);
	}

	void handleUpdateConfigurationRequest() {
		server.keepAlive(false);

		// server.arg("plain") reads the raw request body, a JSON string
		// in our case; this has nothing to do with multipart encoding
		if (eeprom::updateFromJSON(tmpContext, server.arg("plain"), NULL)) {
			server.send(200, "text/plain", "OK");
		} else {
			server.send(400, "text/plain", "Configuration was invalid.");
		}
	}

	void handleGetFileRequest(const String &filepath) {
		File file = LittleFS.open(filepath, "r");
		server.streamFile(file, mime::getContentType(filepath));
		file.close();
	}

	void setup() {
		if (!LittleFS.begin()) {
			Serial.println("An Error has occurred while mounting LittleFS");
			return;
		}

		server.serveStatic("/assets", LittleFS, "/assets", "max-age=2592000"); // 30d

		server.on("/", []() { handleGetFileRequest("/index.html"); });

		server.on("/config", []() {
			switch (server.method()) {
			case HTTP_GET:
				handleGetConfigurationRequest();
				break;

			case HTTP_POST:
				handleUpdateConfigurationRequest();
				break;

			default:
				server.send(400, "text/plain", "invalid method, allowed: GET, POST");
			}
		});

		server.on("/save", []() {
			switch (server.method()) {
			case HTTP_POST:
				eeprom::save(tmpContext);
				server.send(200, "text/plain", "OK");
				break;

			default:
				server.send(400, "text/plain", "invalid method, allowed: POST");
			}
		});

		server.on("/sleep", []() {
			switch (server.method()) {
			case HTTP_POST:
				global::goToSleep = true;
				server.send(200, "text/plain", "OK");
				break;

			default:
				server.send(400, "text/plain", "invalid method, allowed: POST");
			}
		});

		server.on("/wakeup", []() {
			switch (server.method()) {
			case HTTP_POST:
				global::goWakeUp = true;
				server.send(200, "text/plain", "OK");
				break;

			default:
				server.send(400, "text/plain", "invalid method, allowed: POST");
			}
		});
	}

	void start(eeprom::data *config) {
		if (config->enableWebserver) {
			server.begin();
			Serial.println("HTTP server started on port 80.");
			running = true;
		}
	}

	void stop() {
		if (running) {
			server.stop();
			Serial.println("HTTP server stopped.");
			running = false;
		}
	}

	void handleRequests(eeprom::data *config) {
		if (running) {
			tmpContext = config;
			server.handleClient();
		}
	}
} // namespace webserver
