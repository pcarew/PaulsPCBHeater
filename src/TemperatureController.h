/*
 * TemperatureController.h
 *
 *  Created on: Apr 28, 2024
 *      Author: PaulCarew
 */

#ifndef TEMPERATURECONTROLLER_H
#define TEMPERATURECONTROLLER_H

#include "Extensions.h"
#include "menu.h"

#define TC_MEASUREMENT_PERIOD 250

class TemperatureController: Implements MenuAction {
	static unsigned long periodEnd;
public:
	static int desiredTemp;						// Target
	static int desiredSlope;					// Speed to target  deg/minute

	static double valueBeingChanged;
	static double prevTempReading;

//	TemperatureController();
//	virtual ~TemperatureController();

	static void update();				// Called from Tasking
	static void valueDetermination();
	static void updateHeater();

								// Profile control
	static int getTemperature();
	static void setTemperature(int targetTemp, int slope);						// Slope is in deg per min (pos. or neg. slope). zero = no slope, move as fast as possible
};

#endif /* TEMPERATURECONTROLLER_H_ */
