/*
 * DisplayControl.h
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#ifndef DISPLAYCONTROLLER_H_
#define DISPLAYCONTROLLER_H_
#include "Arduino.h"
#include "pos/pos.h"

#define QUEUESIZE 8

class DisplayController {
public:

	DisplayController();
	virtual ~DisplayController();
};

#define LED_PIN   A4
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

	static LEDMode ledStatus;
	static void setup();
	static void update();

	static void ledEvent(LEDMode mode);
};

class Graphics{
private:
public:
};
#endif /* DISPLAYCONTROLLER_H_ */
