/*
 * AmbientMonitor.cpp
 *
 *  Created on: Jun 26, 2024
 *      Author: PaulCarew
 */

#include <arduino.h>
#include "TemperatureMonitoring.h"
#include "AmbientMonitor.h"

AmbientMonitor::AmbientMonitor() { }
AmbientMonitor::~AmbientMonitor() { }


void AmbientMonitor::update(){				// Called statically from Tasking
	static unsigned long periodEnd	= 0;
	double ambientReading;

	volatile unsigned long timeNow = millis();
	if(timeNow > periodEnd){
		periodEnd = timeNow + AM_MEASUREMENT_PERIOD;
		ambientReading = TemperatureMonitoring::ambient.getTemperature();
		if(ambientReading > AMBIENTWARNING){
				// Signal UI of warning
		}else if(ambientReading > AMBIENTDANGER){
				// Signal UI of Danger
				// disable the heater
		}else{		// All ok.
				// Remove any UI overheating indication
		}
	}
}
