#include "blinketto_mode_on.h"
#include <Arduino.h>

BlinkettoModeOn::BlinkettoModeOn() {
}

bool BlinkettoModeOn::canRun(Blinketto &blinketto) {
	return true;
}

void BlinkettoModeOn::run(Blinketto &blinketto, BlinkettoInterrupter &interrupter) {
	blinketto.transitionToBitmap(0xFF, 5, 100, 50);
	while (true) {
		STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)
		yield();
	}
}
