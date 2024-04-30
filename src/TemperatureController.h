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

#define TC_MEASUREMENT_PERIOD 1000

class TemperatureController: Implements MenuAction {
	static unsigned long periodEnd;
public:
	static int desiredTemp;						// Target
	static int desiredSlope;					// Speed to target  deg/minute

	static unsigned char valueBeingChanged;
	static double prevTempReading;

//	TemperatureController();
//	virtual ~TemperatureController();

	// UI
//	void menuAction(int param) ;
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT

	static void update();				// Called from Tasking
	static void TemperatureController::valueDetermination();
	static void TemperatureController::updateHeater();

								// Profile control
	static void setTempeature(int targetTemp, int slope);						// Slope is in deg per min (pos. or neg. slope). zero = no slope, move as fast as possible
};

#endif /* TEMPERATURECONTROLLER_H_ */
