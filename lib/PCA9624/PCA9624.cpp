/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2013, ISR University of Coimbra.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the ISR University of Coimbra nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Gonçalo Cabrita on 02/04/2013
 *********************************************************************/

//#if ARDUINO >= 100
//#include <Arduino.h>
//#else
//#include <WProgram.h>
//#endif
#include "Arduino.h"
#include "PCA9624.h"

PCA9624::PCA9624(char addr) {
	this->address_[0] = addr;
}

void PCA9624::writeMode1(char autoinc, char sleep, char sub1, char sub2, char sub3, char allcall, char addr) {
	this->autoinc_ = autoinc;
	this->sleep_ = sleep;
	this->address_state_[0] = sub1;
	this->address_state_[1] = sub2;
	this->address_state_[3] = sub3;
	this->address_state_[4] = allcall;

	char mode1 = 0x00;

	mode1 = allcall;
	mode1 += sub1 << 1;
	mode1 += sub2 << 2;
	mode1 += sub3 << 3;
	mode1 += sleep << 4;
	mode1 += autoinc << 5;

	this->writeRegister(PCA9624_REG_MODE1, mode1, addr);
}

char PCA9624::readMode1() {
	char mode1 = this->readRegister(PCA9624_REG_MODE1);

	this->address_state_[3] = mode1 & 0x01; // All Call
	this->address_state_[2] = (mode1 >> 1) & 0x01; // Sub3
	this->address_state_[1] = (mode1 >> 2) & 0x01; // Sub2
	this->address_state_[0] = (mode1 >> 3) & 0x01; // Sub3
	this->sleep_ = (mode1 >> 4) & 0x01;
	this->autoinc_ = (mode1 >> 5) & 0x07;

	return mode1;
}

void PCA9624::writeAddress(char addr, char value) {
	if (addr >= PCA9624_ADDR_SUBADR1 && addr <= PCA9624_ADDR_ALLCALLADR) {
		this->address_[addr] = value;
		this->writeRegister(addr+0x0D, value);
	}
}

char PCA9624::readAddress(char addr) {
	if (addr >= PCA9624_ADDR_SUBADR1 && addr <= PCA9624_ADDR_ALLCALLADR) {
		char value = this->readRegister(addr+0x0D);
		this->address_[addr] = value;
		return value;
	}

	return 0;
}

char PCA9624::getAddressFlag(char addr) {
	if (addr >= PCA9624_ADDR_SUBADR1 && addr <= PCA9624_ADDR_ALLCALLADR) {
		return this->address_state_[addr-1];
	}

	return 0;
}

void PCA9624::writeMode2(char dmblk, char invrt, char och, char addr) {
	this->dmblk_ = dmblk;
	this->invrt_ = invrt;
	this->och_ = och;

	char mode2 = 0x00;

	mode2 += 0x5;
	mode2 += och << 3;
	mode2 += invrt << 4;
	mode2 += dmblk << 5;

	this->writeRegister(PCA9624_REG_MODE2, mode2, addr);
}

char PCA9624::readMode2() {
	char mode2 = this->readRegister(PCA9624_REG_MODE2);

	this->och_   = (mode2 >> 3) & 0x01;
	this->invrt_ = (mode2 >> 4) & 0x01;
	this->dmblk_ = (mode2 >> 5) & 0x01;

	return mode2;
}

void PCA9624::writeLedOut(char led0, char led1, char led2, char led3, char led4, char led5, char led6, char led7, char addr) {
	this->ledout_[0] = led0;
	this->ledout_[1] = led1;
	this->ledout_[2] = led2;
	this->ledout_[3] = led3;
	this->ledout_[4] = led4;
	this->ledout_[5] = led5;
	this->ledout_[6] = led6;
	this->ledout_[7] = led7;

	char ledout;

	ledout = led0;
	ledout += led1 << 2;
	ledout += led2 << 4;
	ledout += led3 << 6;

	this->writeRegister(PCA9624_REG_LEDOUT0, ledout, addr);

	ledout = led4;
	ledout += led5 << 2;
	ledout += led6 << 4;
	ledout += led7 << 6;

	this->writeRegister(PCA9624_REG_LEDOUT1, ledout, addr);
}

void PCA9624::readLedOut() {
	char ledout;

	ledout = this->readRegister(PCA9624_REG_LEDOUT0);

	this->ledout_[0] = ledout & 0x03;
	this->ledout_[1] = (ledout >> 2) & 0x03;
	this->ledout_[2] = (ledout >> 4) & 0x03;
	this->ledout_[3] = (ledout >> 6) & 0x03;

	ledout = this->readRegister(PCA9624_REG_LEDOUT1);

	this->ledout_[4] = ledout & 0x03;
	this->ledout_[5] = (ledout >> 2) & 0x03;
	this->ledout_[6] = (ledout >> 4) & 0x03;
	this->ledout_[7] = (ledout >> 6) & 0x03;
}

char PCA9624::getLedOut(char led) {
	if (led >= PCA9624_LED_0 && led <= PCA9624_LED_7) {
		return this->ledout_[led];
	}

	return 0;
}

void PCA9624::writePWM(char led, char duty, char addr) {
	if (led >= PCA9624_LED_0 && led <= PCA9624_LED_ALL) {
		this->writeRegister(PCA9624_REG_PWM0 + led, duty, addr);
	}
}

void PCA9624::writeAllPWMs(char leds[8], char groupPWM, char groupFreq) {
	Wire.beginTransmission(address_[PCA9624_ADDR_CHIP]);
	Wire.write(PCA9624_REG_PWM0 + 224); // 224 = 11100000 = enable auto increment

	// send all LED brightness levels
	for (uint8_t i = 0; i < 8; ++i) {
		Wire.write(leds[i]);
	}

	// group group controls
	Wire.write(groupPWM);
	Wire.write(groupFreq);

	Wire.endTransmission();
}

char PCA9624::readPWM(char led) {
	if (led >= PCA9624_LED_0 && led <= PCA9624_LED_7) {
		return this->readRegister(PCA9624_REG_PWM0 + led);
	}
	return 0x00;
}

void PCA9624::writeGroupFrequency(char freq, char addr) {
	this->writeRegister(PCA9624_REG_GRPFREQ, freq, addr);
}

char PCA9624::readGroupFrequency() {
	return this->readRegister(PCA9624_REG_GRPFREQ);
}

void PCA9624::writeGroupPWM(char pwm, char addr) {
	this->writeRegister(PCA9624_REG_GRPPWM, pwm, addr);
}

char PCA9624::readGroupPWM() {
	return this->readRegister(PCA9624_REG_GRPPWM);
}

void PCA9624::writeRegister(char reg, char value, char addr) {
	// Serial.print("writeRegister(");
	// Serial.print(reg, BIN);
	// Serial.print(", ");
	// Serial.print(value, BIN);
	// Serial.print(", ");
	// Serial.print(address_[addr], BIN);
	// Serial.println(")");

	Wire.beginTransmission(this->address_[addr]);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
	delay(10);
}

char PCA9624::readRegister(char reg, char addr) {
	char reply = 0;

	Wire.beginTransmission(this->address_[addr]);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(this->address_[addr], 1);
	delayMicroseconds(10);
	if (Wire.available() >= 1) {
		reply = int(Wire.read());
	}

	// Serial.print("readRegister(");
	// Serial.print(reg, BIN);
	// Serial.print(", ");
	// Serial.print(address_[addr], BIN);
	// Serial.print(") = ");
	// Serial.print(reply, BIN);
	// Serial.println("");

	return reply;
}
