/*
 * LEDControl.cpp
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#include "LEDController.h"


LEDController::LEDMode LEDController::ledMode		= LEDMode::Off;
LEDProfile LEDController::ledProfile					= {0,0,0,0};
LEDController::LEDState LEDController::ledCycleState = LEDStoppedSt;
unsigned long LEDController::ledEventTimer					= 0;
unsigned char LEDController::flashCnt						= 0;
const PROGMEM LEDProfile LEDController::ledProfiles[] = {		// 1 profile per LED mode
		//T,	D,	Cnt,	CycleTime
		{125, (unsigned char)50, (unsigned char)6, 3000},	// Self Test
		{0,0,0,0},			// Off
		{2000,50,1,2000},	// Zero Crossing
		{1000,50,2,2000},	// Heater On
		{125,50,3,2000},	// Overheat Warning
		{125,50,3,375},		// Overheated - Shutdown
};

const PROGMEM LEDController::FSMEntry LEDController::fsm[NUMLEDSTATES][NUMLEDEVENTS] = {
	// CycleStartEv				LedOnCompleteEv					MoreEv						LedQuietEv						CycleOffEv
	{// LED Stopped State
	{fsmActStartCycle,LEDOnSt},	{fsmActNoAction,LEDStoppedSt},{fsmActNoAction,LEDStoppedSt},{fsmActNoAction,LEDStoppedSt},	{fsmActNoAction,LEDStoppedSt}
	},
	{// LED On State
	{fsmActNoAction,LEDOnSt},	{fsmActTurnLedOff,LEDOffSt},	{fsmActNoAction,LEDOnSt},	{fsmActNoAction,LEDOnSt},		{fsmActNoAction,LEDOnSt}
	},
	{// LED Off State
	{fsmActNoAction,LEDOffSt},	{fsmActNoAction,LEDOffSt},		{fsmActTurnLedOn,LEDOnSt},	{fsmActStartQuiet,LEDQuietSt},	{fsmActNoAction,LEDOffSt}
	},
	{// LED Quiet State
	{fsmActStartCycle,LEDOnSt},	{fsmActNoAction,LEDQuietSt},	{fsmActNoAction,LEDQuietSt},{fsmActNoAction,LEDQuietSt},	{fsmActNoAction,LEDStoppedSt}
	}
};

void LEDController::setup(){
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, LOW);
//	LEDController::ledSetMode(LEDMode::SelfTest);
	LEDController::ledSetMode(LEDMode::Off);
}

void LEDController::ledSetMode(LEDMode mode){
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
		LEDController::ledMode = mode;
//	}
    memcpy_P (&ledProfile, &LEDController::ledProfiles[mode], sizeof(LEDProfile));
}

void LEDController::update(){
	unsigned long time;
	if(ledMode != Off){					// If we're not 'Off', there will always be a timer running
		time = millis();
		if(time > LEDController::ledEventTimer){
			LEDEvent event = detectEvent();
			fsmHandler(event);
		}
	}
}
void LEDController::displayLedProfile(){

	unsigned int t			= ledProfile.wavelengthT;
	unsigned int ledOnTime	= (unsigned int)((long)t * (long)ledProfile.duty / 100l);
	unsigned char n			= (unsigned char) ledProfile.flashCnt;
	unsigned int repeat		= ledProfile.cycleTime;

	Serial.print(F("LED Profile: "));
	Serial.print(F(" T: "));Serial.print(t);
	Serial.print(F(" D: "));Serial.print(ledOnTime);
	Serial.print(F(" N: "));Serial.print(n);
	Serial.print(F(" L: "));Serial.print(repeat);
	Serial.println();
}

LEDController::LEDEvent LEDController::detectEvent(){
	switch(ledCycleState){
	case LEDStoppedSt:	return CycleStartEv; break;
	case LEDOnSt:		return LedOnCompleteEv; break;
	case LEDOffSt:
						if(flashCnt >0) return MoreEv;
						else return LedQuietEv;
						break;
	case LEDQuietSt:	if(ledProfile.wavelengthT == 0) return CycleOffEv;
						else return CycleStartEv;
						break;
	}
}

void LEDController::fsmHandler(LEDController::LEDEvent event){
	FSMEntry currentStateEvent;
    memcpy_P (&currentStateEvent, &LEDController::fsm[LEDController::ledCycleState][event], sizeof(FSMEntry));

	Serial.print(F("LED FSM S: "));Serial.print(LEDController::ledCycleState);Serial.print(F(" E:"));Serial.print(event);
	Serial.print(F(" fc: ")); Serial.println(LEDController::flashCnt);
    currentStateEvent.action(event);
   	ledCycleState = currentStateEvent.nextState;
//	Serial.print(F("  -> new LED FSM S: "));Serial.print(LEDController::ledCycleState); Serial.print(F(" fc: ")); Serial.println(LEDController::flashCnt);

}


void LEDController::fsmActNoAction(LEDController::LEDEvent event){
//	Serial.print(F("LED FSM Err S: "));Serial.print(LEDController::ledCycleState);Serial.print(F(" :"));Serial.println(event);
//	LEDController::displayLedProfile();
}

void LEDController::fsmActStartCycle(LEDController::LEDEvent event){
	unsigned int ledOnTime	= (unsigned int)((long)ledProfile.wavelengthT * (long)ledProfile.duty / 100l);

	LEDController::flashCnt			= ledProfile.flashCnt -1;

	LEDController::ledEventTimer	= millis() + ledOnTime;
	digitalWrite(LED_PIN, HIGH);
}

void LEDController::fsmActTurnLedOn(LEDController::LEDEvent event){
	unsigned int ledOnTime	= (unsigned int)((long)ledProfile.wavelengthT * (long)ledProfile.duty / 100l);

	LEDController::ledEventTimer	= millis() + ledOnTime;

	digitalWrite(LED_PIN, HIGH);
	if(LEDController::flashCnt > 0) LEDController::flashCnt--;
}

void LEDController::fsmActTurnLedOff(LEDController::LEDEvent event){
	unsigned int t			= ledProfile.wavelengthT;
	unsigned int ledOnTime	= (int)((long)t * (long)ledProfile.duty / 100l);

	LEDController::ledEventTimer	= millis() + (t-ledOnTime);
	digitalWrite(LED_PIN, LOW);
}

void LEDController::fsmActStartQuiet(LEDController::LEDEvent event){
	unsigned int t			= ledProfile.wavelengthT;
	unsigned char n			= (unsigned int) ledProfile.flashCnt;
	unsigned int repeat		= ledProfile.cycleTime;

	LEDController::ledEventTimer	= millis() + (repeat - n*t);
}
