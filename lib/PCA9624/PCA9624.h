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

#include "Wire.h"

#ifndef PCA9624_h
#define PCA9624_h

// Registers on the PCA9624
#define PCA9624_REG_MODE1       0x00
#define PCA9624_REG_MODE2       0x01
#define PCA9624_REG_PWM0        0x02
#define PCA9624_REG_PWM1        0x03
#define PCA9624_REG_PWM2        0x04
#define PCA9624_REG_PWM3        0x05
#define PCA9624_REG_PWM4        0x06
#define PCA9624_REG_PWM5        0x07
#define PCA9624_REG_PWM6        0x08
#define PCA9624_REG_PWM7        0x09
#define PCA9624_REG_GRPPWM      0x0A
#define PCA9624_REG_GRPFREQ     0x0B
#define PCA9624_REG_LEDOUT0     0x0C
#define PCA9624_REG_LEDOUT1     0x0D
#define PCA9624_REG_SUBADR1     0x0E
#define PCA9624_REG_SUBADR2     0x0F
#define PCA9624_REG_SUBADR3     0x10
#define PCA9624_REG_ALLCALLADR  0x11

// Auto increment on the PCA9624
#define PCA9624_AI_OFF                      0
#define PCA9624_AI_ALL_REGISTERS            1
#define PCA9624_AI_INDIVIDUAL               2
#define PCA9624_AI_GLOBAL                   3
#define PCA9624_AI_INDIVIDUAL_AND_GLOBAL    4

// Output on the PCA9624
#define PCA9624_OUTPUT_CHANGE_ON_STOP       0
#define PCA9624_OUTPUT_CHANGE_ON_ACK        1

// Dim Blink
#define PCA9624_DIM                         0
#define PCA9624_BLINK                       1

// LED output on the PCA9624
#define PCA9624_LED_OFF                     0
#define PCA9624_LED_ON_NO_CONTROL           1
#define PCA9624_LED_INDIVIDUAL              2
#define PCA9624_LED_INDIVIDUAL_AND_GROUP    3

// Addresses
#define PCA9624_ADDR_CHIP                   0
#define PCA9624_ADDR_SUBADR1                1
#define PCA9624_ADDR_SUBADR2                2
#define PCA9624_ADDR_SUBADR3                3
#define PCA9624_ADDR_ALLCALLADR             4

// LEDs on the PCA9624
#define PCA9624_LED_0                       0
#define PCA9624_LED_1                       1
#define PCA9624_LED_2                       2
#define PCA9624_LED_3                       3
#define PCA9624_LED_4                       4
#define PCA9624_LED_5                       5
#define PCA9624_LED_6                       6
#define PCA9624_LED_7                       7
#define PCA9624_LED_ALL                     8

class PCA9624 {
	public:
	PCA9624(char addr);

	void writeMode1(char autoinc, char sleep, char sub1, char sub2, char sub3, char allcall, char addr = PCA9624_ADDR_CHIP);
	char readMode1();

	char getAutoIncrement() { return this->autoinc_; };
	char getSleep()         { return this->sleep_;   };
	char getDimBlink()      { return this->dmblk_;   };
	char getInvert()        { return this->invrt_;   };
	char getOutputChange()  { return this->och_;     };

	char getAddressFlag(char addr);

	void writeAddress(char addr, char value);
	char readAddress(char addr);

	void writeMode2(char dmblk, char invrt, char och, char addr = PCA9624_ADDR_CHIP);
	char readMode2();

	void writeLedOut(char led0, char led1, char led2, char led3, char led4, char led5, char led6, char led7, char addr = PCA9624_ADDR_CHIP);
	void readLedOut();
	char getLedOut(char led);

	void writePWM(char led, char duty, char addr = PCA9624_ADDR_CHIP);
	char readPWM(char led);

	void writeAllPWMs(char leds[8], char groupPWM, char groupFreq);

	void writeGroupFrequency(char freq, char addr = PCA9624_ADDR_CHIP);
	char readGroupFrequency();

	void writeGroupPWM(char pwm, char addr = PCA9624_ADDR_CHIP);
	char readGroupPWM();

	private:
	char address_[5];

	// PCA9624 mode1 settings
	char autoinc_;
	char sleep_;
	char address_state_[4];

	// PCA9624 mode2 settings
	char dmblk_;
	char invrt_;
	char och_;

	// PCA9624 ledout settings
	char ledout_[8];

	void writeRegister(char Register, char value, char addr = PCA9624_ADDR_CHIP);
	char readRegister(char Register, char addr = PCA9624_ADDR_CHIP);
};

#endif
