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
	static unsigned long periodEnd	= millis()+10000l;						// Start monitoring after the 1st 10 seconds
	double ambientReading;

	volatile unsigned long timeNow = millis();
	if(timeNow > periodEnd){
		periodEnd = timeNow + AM_MEASUREMENT_PERIOD;
		ambientReading = TemperatureMonitoring::ambient.getTemperature();
		if(ambientReading > AMBIENTDANGER){
				// Signal UI of Danger
			LEDController::ledSetMode(LEDController::LEDMode::AmbientDanger);
				// disable the heater
			HeaterController::heaterEnabled = false;
		}else if(ambientReading > AMBIENTWARNING){
				// Signal UI of warning
			LEDController::ledSetMode(LEDController::LEDMode::AmbientWarning);
		}else{		// All ok.
				// Remove any UI overheating indication
			LEDController::ledSetMode(LEDController::LEDMode::AmbientCancel);
		}
	}
}
