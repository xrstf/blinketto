#pragma once

#include "blinketto.h"

#define STOP_IF_PLUG_OR_MODE_CHANGED(interrupter, blinketto)                                                           \
	if (interrupter.shouldInterrupt(blinketto)) {                                                                      \
		return;                                                                                                        \
	}

class BlinkettoInterrupter {
public:
	virtual bool shouldInterrupt(Blinketto &blinketto) = 0;
};
