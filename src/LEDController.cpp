/*
 * LEDControl.cpp
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#include "LEDController.h"


LEDController::LEDMode LEDController::ledStatus = LEDMode::SelfTest;
const PROGMEM LEDProfile LEDController::ledProfiles[] = {		// 1 profile per LED mode
		//T,	D,	Cnt,	CycleTime
		{250, (unsigned char)50, (unsigned char)3, 750},	// Self Test
		{0,0,0,0},			// Off
		{2000,50,1,2000},	// Zero Crossing
		{1000,50,2,2000},	// Heater On
		{125,50,3,2000},	// Overheat Warning
		{125,50,3,375},		// Overheated - Shutdown
};

void LEDController::setup(){
	pinMode(LED_PIN, OUTPUT);
	LEDController::ledStatus = LEDMode::SelfTest;
	for(int i = 0; i<3; i++){
		digitalWrite(LED_PIN,HIGH);
		delay(125);
		digitalWrite(LED_PIN,LOW);
		delay(125);
	}
}
void LEDController::ledEvent(LEDMode mode){
	/*
	switch(mode){
	case LEDMode::Off:
	case LEDMode::ZeroCrossing:
	case LEDMode::HeaterOn:
	case LEDMode::AmbientWarning:
	case LEDMode::AmbientDanger:
		break;
	}
	*/
//	if(mode >= LEDController::ledStatus){
		LEDController::ledStatus = mode;
//	}
}

void LEDController::update(){
	unsigned long time = millis();
	static unsigned long displayTime = 0l;
	if(time > displayTime){
		displayTime = time+5000l;
		LEDController::displayCycle();
	}
}
void LEDController::displayCycle(){
	LEDProfile profile;
	static int id = 0;
	static unsigned long displayTime = 0l;
	unsigned long time = millis();

    memcpy_P (&profile, &LEDController::ledProfiles[LEDController::ledStatus], sizeof(LEDProfile));
//    memcpy_P (&profile, &LEDController::ledProfiles[(id++)%6], sizeof(LEDProfile));
													;

	int t			= profile.wavelengthT;
	int ledOnTime	= (int)((long)t * (long)profile.duty / 100l);
	int n			= (int) profile.flashCnt;
	int repeat		= profile.cycleTime;

	Serial.print(F("LED Profile: "));
	Serial.print(F(" T: "));Serial.print(t);
	Serial.print(F(" D: "));Serial.print(ledOnTime);
	Serial.print(F(" N: "));Serial.print(n);
	Serial.print(F(" L: "));Serial.print(repeat);
	Serial.println();

	for(int i=0;i<n;i++){
		digitalWrite(LED_PIN, HIGH);
		delay(ledOnTime);
		digitalWrite(LED_PIN, LOW);
		delay(t-ledOnTime);
	}
}
