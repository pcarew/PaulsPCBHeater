/*
 * TemperatureController.cpp
 *
 *  Created on: Apr 28, 2024
 *      Author: PaulCarew
 */

#include "TemperatureController.h"
#include "TemperatureMonitoring.h"
#include "FuzzyTemp.h"
#include "HeaterController.h"

unsigned long TemperatureController::periodEnd	= 0;
int TemperatureController::desiredTemp			= 0;					// Target
int TemperatureController::desiredSlope			= 0;					// Speed to target  deg/minute
unsigned char TemperatureController::valueBeingChanged	= 0;
double TemperatureController::prevTempReading	= 0;

/*
TemperatureController::TemperatureController() {
	// TODO Auto-generated constructor stub

}

TemperatureController::~TemperatureController() {
	// TODO Auto-generated destructor stub
}
*/

void TemperatureController::setTempeature(int targetTemp, int slope){						// Slope is in deg per min (pos. or neg. slope). zero = no slope, move as fast as possible
	TemperatureController::desiredTemp = targetTemp;
	TemperatureController::desiredSlope = slope;
}

void TemperatureController::update(){				// Called from Tasking

	volatile unsigned long timeNow = millis();
	if(timeNow > periodEnd){
		periodEnd = timeNow + TC_MEASUREMENT_PERIOD;
		valueDetermination();					// Will either use slope or target temperature
		updateHeater();
//	showRpm(timeNow);
//	showRevs(timeNow);
//	showLed();
//	go(timeNow);
//	speedIn(timeNow);

	 // Get current Bottom Temp
	}

}

void TemperatureController::valueDetermination() {
		double tempReading = TemperatureMonitoring::brdBot.getTemperature();
//		double tempErrPercent = (TemperatureController::desiredTemp - tempReading) / TemperatureController::desiredTemp;
			valueBeingChanged = (unsigned char) FuzzyTemp::getValueChangePercent(tempReading,(double)TemperatureController::desiredTemp);
			/* AJPC Ignore using slope while in development
		if(abs(tempErrPercent) < 10.0){	// Use Target Temp
			valueBeingChanged = (unsigned char) FuzzyTemp::getValueChangePercent(tempReading,(double)TemperatureController::desiredTemp);
		}else{							// Use Slope
			double actualSlope = (tempReading - prevTempReading)/TC_MEASUREMENT_PERIOD;
			valueBeingChanged = (unsigned char) FuzzyTemp::getValueChangePercent(actualSlope,(double)TemperatureController::desiredSlope);
		}
		*/
		prevTempReading = tempReading;
}
void TemperatureController::updateHeater(){
	HeaterController::setPercentagePwr(valueBeingChanged);									// Called by Temperature control to set required heater power

	/*
		if(pwm<MINIMUMPWM)pwm = MINIMUMPWM;
		double change = getRpmChangePercent(rpm, desiredSpeed);
		change = 1.0 + (change/100.0);
		pwm = (unsigned int)((double)pwm * change);
		if(pwm>255)pwm = 255;
		if(pwm<2)pwm = 2;
    	analogWrite(pwmPin, pwm);
    	*/
}

