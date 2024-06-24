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

#define TC_MEASUREMENT_PERIOD 500

class TemperatureController: Implements MenuAction {
	static unsigned long periodEnd;
public:
	static int targetTemp;						// Target Temp from top sensor
	static int targetPower;						// Target Temp from top sensor
	static int guardTemp;						// Guard  Temp from bottom sensor
	static int guardPower;						// Guard  Temp from bottom sensor
	static int desiredSlope;					// Speed to target  deg/minute

	static double powerSetting;
	static double prevTempReading;

	static double prevTargetError;
	static double prevGuardError;

//	TemperatureController();
//	virtual ~TemperatureController();

	static void update();				// Called from Tasking
	static void valueDetermination();
	static void updateHeater();

								// Profile control
	static int getTargetTemperature();
	static void setTemperature(int targetTemp, int guard, int slope);						// Slope is in deg per min (pos. or neg. slope). zero = no slope, move as fast as possible
};

#endif /* TEMPERATURECONTROLLER_H_ */
