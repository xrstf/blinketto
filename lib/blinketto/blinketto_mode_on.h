#pragma once

#include "blinketto.h"
#include "blinketto_interrupter.h"

class BlinkettoModeOn {
public:
	BlinkettoModeOn();
	bool canRun(Blinketto &blinketto);
	void run(Blinketto &blinketto, BlinkettoInterrupter &interrupter);
};
