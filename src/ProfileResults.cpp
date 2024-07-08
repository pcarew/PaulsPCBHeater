/*
 * ProfileResults.cpp
 *
 *  Created on: Jul 5, 2024
 *      Author: PaulCarew
 */

#include <arduino.h>
#include "ProfileResults.h"

ProfileResults::ProfileResults() {
}

ProfileResults::~ProfileResults() {
}

void ProfileResults::tempDataPacket(unsigned long time,unsigned guard,unsigned guardCurrent, unsigned target,unsigned targetCurrent){
// 	Output to serial for logging
	char buff[20];
	sprintf_P(buff,PSTR("%u,%u,%u,%u\n"),guard,guardCurrent,target,targetCurrent);
//	Serial.println(strlen(buff));
	Serial.print(buff);
	// Save to EEPROM?


	// Send to ProfileGrapher for display
}