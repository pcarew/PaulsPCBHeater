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
int TemperatureController::targetTemp			= 0;					// Target
int TemperatureController::targetPower			= 0;					// Target
int TemperatureController::guardTemp			= 0;					// Guard
int TemperatureController::guardPower			= 0;					// Guard
int TemperatureController::desiredSlope			= 0;					// Speed to target  deg/minute
double TemperatureController::powerSetting		= 0;
double TemperatureController::prevTargetError	= 0;
double TemperatureController::prevGuardError	= 0;
//double TemperatureController::prevTempReading	= 0;

/*
TemperatureController::TemperatureController() {
	// TODO Auto-generated constructor stub

}

TemperatureController::~TemperatureController() {
	// TODO Auto-generated destructor stub
}
*/

int TemperatureController::getTargetTemperature(){
	return TemperatureController::targetTemp;
}
void TemperatureController::setTemperature(int targetTemp, int guardTemp, int slope){						// Slope is in deg per min (pos. or neg. slope). zero = no slope, move as fast as possible
	TemperatureController::targetTemp = targetTemp;
	TemperatureController::guardTemp = guardTemp;
	TemperatureController::desiredSlope = slope;
}

void TemperatureController::update(){				// This is the background *Thread* Called from Tasking

	volatile unsigned long timeNow = millis();
	if(timeNow > periodEnd){
		periodEnd = timeNow + TC_MEASUREMENT_PERIOD;
		valueDetermination();					// Will either use slope or target temperature
		updateHeater();
	}
}

void TemperatureController::valueDetermination() {
		double topReading = TemperatureMonitoring::brdTop.getTemperature();
		double bottomReading = TemperatureMonitoring::brdBot.getTemperature();
//		double tempErrPercent = (TemperatureController::desiredTemp - tempReading) / TemperatureController::desiredTemp;


		TemperatureController::targetPower = FuzzyTemp::getPowerPercent(topReading,(double)TemperatureController::targetTemp, &TemperatureController::prevTargetError) ;
		TemperatureController::guardPower = FuzzyTemp::getPowerPercent(bottomReading,(double)TemperatureController::guardTemp,&TemperatureController::prevGuardError) ;

		TemperatureController::powerSetting = min(TemperatureController::targetPower,TemperatureController::guardPower);
//		Serial.print(F("T%:"));Serial.print(targetPower);Serial.print(F(" G%:"));Serial.print(guardPower);Serial.print(F(" Min:"));Serial.println(min(targetPower,guardPower));

		/* AJPC Ignore using slope while in development
		if(abs(tempErrPercent) < 10.0){	// Use Target Temp
			valueBeingChanged = (unsigned char) FuzzyTemp::getValueChangePercent(tempReading,(double)TemperatureController::desiredTemp);
		}else{							// Use Slope
			double actualSlope = (tempReading - prevTempReading)/TC_MEASUREMENT_PERIOD;
			valueBeingChanged = (unsigned char) FuzzyTemp::getValueChangePercent(actualSlope,(double)TemperatureController::desiredSlope);
		}
//		prevTempReading = bottomReading;
		*/
}
void TemperatureController::updateHeater(){
	HeaterController::setPercentagePwr( (powerSetting<0?(unsigned char)0:(unsigned char) powerSetting) );				// Heater cant accept negative values
}
