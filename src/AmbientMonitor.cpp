/*
 * AmbientMonitor.cpp
 *
 *  Created on: Jun 26, 2024
 *      Author: PaulCarew
 */

#include <arduino.h>
#include "TemperatureMonitoring.h"
#include "AmbientMonitor.h"
#include "LEDController.h"
#include "HeaterController.h"

AmbientMonitor::AmbientMonitor() { }
AmbientMonitor::~AmbientMonitor() { }


void AmbientMonitor::update(){				// Called statically from Tasking
	unsigned long timeNow = millis();
	static unsigned long periodEnd = timeNow + AM_DELAY_START;
	double ambientReading;

	if(timeNow > periodEnd){
		periodEnd = timeNow + AM_MEASUREMENT_PERIOD;
		ambientReading = TemperatureMonitoring::ambient.getTemperature();
		if(ambientReading > AMBIENTDANGER){
				// Signal UI of Danger
			LEDController::ledSetMode(LEDController::LEDMode::AmbientDanger);
				// disable the heater
			HeaterController::heaterEnabled = false;
			Serial.print(F("Ambient Danger "));Serial.println(ambientReading);
		}else if(ambientReading > AMBIENTWARNING){
				// Signal UI of warning
			LEDController::ledSetMode(LEDController::LEDMode::AmbientWarning);
			Serial.print(F("Ambient Warning "));Serial.println(ambientReading);
		}else{		// All ok.
				// Remove any UI overheating indication
			LEDController::ledSetMode(LEDController::LEDMode::AmbientCancel);
		}
	}
}
