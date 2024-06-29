#include "HeaterController.h"

#include "arduino.h"

#include "LEDController.h"
#include "pos/pos.h"
#include "RotarySelector.h"

	// Init statics
unsigned long HeaterController::periodEnd		= 0;
unsigned int HeaterController::zeroCount		= 0;
unsigned int HeaterController::zHz				= 0;			// Holds the calculated AC frequency based upon AC Zero crossings. It should be 60Hz
bool HeaterController::heaterEnabled			= false;
unsigned char HeaterController::powerCounter	= 0;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
unsigned char HeaterController::powerPercentage= 0;			// The percentage power being applied
int HeaterController::prevZeroCrossingState	= LOW;

	// Menu buffers
extern char dispBuff[];
const char *HeaterController::fmt = "Heater Control";


// Creation /Setup
HeaterController::HeaterController(){
}

void HeaterController::setup(){
	// Pin Change Interrupt setup
	PCICR = PCICR|PCIR_PORTC;
	// Port C Pins
	PCMSK1 = PCMSK_Z;

	pinMode(HTR_PIN, OUTPUT);
	HeaterController::heaterEnable(false);					//  initially disable heater output
}

// Background Process, called from Task
// Actual Heater Firing is driven from ISR
void HeaterController::update(){
		unsigned long timeNow = millis();
		hzDetermination(timeNow);
}

void HeaterController::hzDetermination(unsigned long currentTime)				// Only accurate if called often enough to detect periodEnd time. IE must be called at least 1 / millisecond or better
{
	if(currentTime >= periodEnd){

		double elapsed = ( (double)currentTime - (double)periodEnd + (double)MEASUREMENT_PERIOD);
		zHz = (int) ( ((double)zeroCount * 1000.0) / elapsed );  //HZMULTIPLIER is count / elapsed time  = avg time per count
//			Serial.print(F("Elapsed:"));Serial.print(elapsed);Serial.print(F(" zCnt:"));Serial.println(zeroCount);

		periodEnd = currentTime + MEASUREMENT_PERIOD;
		zeroCount = 0;
	}
}


//Control
void HeaterController::heaterEnable(bool enable){						// Enable/disable heater output
	heaterEnabled = enable;
}

void HeaterController::setPercentagePwr(unsigned char percentage){
	if(percentage > 100)
		HeaterController::powerPercentage = 100;
	else
		HeaterController::powerPercentage = percentage;

	HeaterController::updatePowerCounter();
}

void HeaterController::updatePowerCounter(){
	HeaterController::powerCounter = (unsigned char)(((unsigned)HeaterController::powerPercentage*FRAMESIZE)/100);	// Turn % into actual power on count
}

void HeaterController::setRawPwr(unsigned char count){					// Called to set required raw power
	if (count > FRAMESIZE){
		HeaterController::powerCounter = FRAMESIZE;
	}else
		HeaterController::powerCounter = count;

	HeaterController::updatePowerPercentage();
}

	void HeaterController::updatePowerPercentage(){						// Sets power percentage based on existing power count
		HeaterController::powerPercentage = (HeaterController::powerCounter*100)/FRAMESIZE;
	}

// Port C ISR Driven
//Following methods are driven via Zero Crossing interrupt
void HeaterController::checkACZeroCrossing(){								// Called from PortC ISR
		int portcValues	= PINC&(PCMSK_Z);
		int nz	= (portcValues&PCMSK_Z);								// New z value
		if(HeaterController::prevZeroCrossingState & ~nz){					// new Z is low, previous was high, we have a zero crossing
			zeroCrossing();
		}
		HeaterController::prevZeroCrossingState = nz;
}

void HeaterController::zeroCrossing(){
	static unsigned ledCount = 0;
	digitalWrite(HTR_PIN,LOW);
	zeroCount++;												// Used for Hz Calculation
	ledCount++;
	frameDetectionAndFiring();
	if(!(ledCount%60)) LEDController::ledEvent(LEDController::LEDMode::ZeroCrossing); //digitalWrite(LED_PIN,!digitalRead(LED_PIN));

	/*
	++ledCntr %=100;	// divide freq by 100 for led visibility
	if(!ledCntr){
		ledV = !ledV;
		digitalWrite(HTR_PIN,ledV);
	}
	*/
}

void HeaterController::fire(){
	if(heaterEnabled)
		digitalWrite(HTR_PIN,HIGH);
}

void HeaterController::frameDetectionAndFiring(){
	static int frameFireCount = 0;
	static int skipCount = FRAMESIZE;								// Skip entire 1st frame
	static int cycleCount = 0;
//				Serial.println(F("Cycle ISR Called"));							// Dangerous for an ISR derived call
	++cycleCount %= FRAMESIZE;

	if(!cycleCount){
																	// Start of a new Frame
		frameFireCount = powerCounter;								// Load the fire count for this frame
		skipCount = FRAMESIZE-frameFireCount;						// # of cycles to skip
	}
	if(skipCount > frameFireCount){
		skipCount--;
	}else{
		frameFireCount--;
		fire();
	}
}


// Used for UI
extern bool cancelled;
extern Display systemDisplay;
extern DisplayText displayElement;

// User Interface for Heater Control. Runs under System UI Thread
void HeaterController::menuAction(int param){
	int i = 200;

	systemDisplay.clear(0,255,0);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	while(!cancelled && i > 0){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time>nextDisplayTime){
			nextDisplayTime = time+500l;
			displayElement.setCol(0);  displayElement.setText((char *)dispBuff);

			sprintf(dispBuff, "Htr:%s ",HeaterController::heaterEnabled?"On":"Off ");
			 displayElement.setRow(1);  displayElement.show();
			sprintf(dispBuff, "zHz: %d \nzCnt:%d ",zHz,zeroCount);
			 displayElement.setRow(2);  displayElement.show();
			sprintf(dispBuff, "Pwr%% : %d  ", HeaterController::powerPercentage);
			 displayElement.setRow(4);  displayElement.show();
			sprintf(dispBuff, "Pwr Raw:%d  ", HeaterController::powerCounter);
			 displayElement.setRow(5);  displayElement.show();
		}
		pause();
	}

//	HeaterControl::heaterEnabled = false;
}

void HeaterController::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR

//			Serial.print(F("HC Dir:"));Serial.println(direction);
	switch(type){
	/*
		case RotaryAction::ROTATE:
//						Serial.print(F("Heater Rotate"));
			switch(direction){
				case RSE::FW:
//						Serial.println(F("Heater Rotate FW"));
					if(HeaterController::powerPercentage<100)
						HeaterController::powerPercentage++;
					break;
				case RSE::RV:
//						Serial.println(F("Heater Rotate RV"));
					if(HeaterController::powerPercentage > 0)
						HeaterController::powerPercentage--;
					break;
				case RSE::NC:
//						Serial.println(F("Heater Rotate NC"));
					break;
			}
			HeaterController::updatePowerCounter();
			break;
			*/
		case RotaryAction::SELECT:
			if(level == ButtonAction::BUTTONLOW){
//						Serial.println(F("Heater ButtonAction Low"));
				HeaterController::heaterEnabled = !HeaterController::heaterEnabled;
			}else{
//						Serial.println(F("Heater ButtonAction HIGH"));
			}
		break;
	}
}
