/*
 * ProfileResults.cpp
 *
 *  Created on: Jul 5, 2024
 *      Author: PaulCarew
 */

#include <arduino.h>
#include "ProfileResults.h"
#include "ProfileGrapher.h"

ProfileResults::ProfileResults() {
}

ProfileResults::~ProfileResults() {
}

void ProfileResults::tempDataPacket(unsigned long time,unsigned guard,unsigned guardCurrent, unsigned target,unsigned targetCurrent){
// 	Output to serial for logging
	char buff[20];
	sprintf_P(buff,PSTR("%u,%u,%u,%u\n"),guard,guardCurrent,target,targetCurrent);
	Serial.print(buff);
	// Save to EEPROM?


	// Send to ProfileGrapher for display
	ProfileGrapher::tempDataPacket(time, guardCurrent, targetCurrent);
}

void ProfileResults::startNewProfile(unsigned guard, unsigned target){

	ProfileGrapher::startNewProfile(guard, target);
}
