/*
 * LEDControl.h
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_
#include "Arduino.h"

#define LED_PIN   A4

class LEDProfile{
private:
public:
	// |+++++_____+++++_____+++++_______________|
	//  <D%-> 						(% of T)
	//  <-Wavelnth T--> 			(in ms)
	//  <----FlashCnt(3)---->		(int cnt)
	//  <----------------CycleTime--(in ms)----->

	int wavelengthT;		// Wavelength time in millis
	unsigned char duty;		// Duty cycle %
	unsigned char flashCnt;	// Number of LED flash cycles
	int cycleTime;			// Overall time  in millis before cycle repeats
};

class LEDController{
private:
public:
	enum  LEDMode{
		SelfTest		= 0,
		Off				= 1,
		ZeroCrossing	= 2,
		HeaterOn		= 3,
		AmbientWarning	= 4,
		AmbientDanger	= 5
	};

	static const LEDProfile ledProfiles[];
	static LEDMode ledStatus;
	static void setup();
	static void update();
	static void displayCycle();

	static void ledEvent(LEDMode mode);
};

#endif /* LEDCONTROLLER_H_ */
