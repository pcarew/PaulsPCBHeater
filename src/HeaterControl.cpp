#include "arduino.h"
#include "pos/pos.h"
#include "HeaterControl.h"

#include "RotarySelector.h"

	// Init statics
unsigned long HeaterControl::periodEnd		= 0;
unsigned int HeaterControl::zeroCount				= 0;
unsigned int HeaterControl::zHz				= 0;			// Holds the calculated AC frequency based upon AC Zero crossings. It should be 60Hz
bool HeaterControl::heaterEnabled			= false;
unsigned char HeaterControl::powerCounter	= 0;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
unsigned char HeaterControl::powerPercentage= 0;			// The percentage power being applied
int HeaterControl::prevZeroCrossingState	= LOW;

	// Menu buffers
extern char dispBuff[];
const char *HeaterControl::fmt = "Heater Control";

#define LED_PIN   A4

// Creation /Setup
HeaterControl::HeaterControl(){
	// Pin Change Interrupt setup
	PCICR = PCICR|PCIR_PORTC;
	// Port C Pins
	PCMSK1 = PCMSK_Z;

	pinMode(LED_PIN, OUTPUT);
	pinMode(HTR_PIN, OUTPUT);
}

void HeaterControl::setup(){
	HeaterControl::heaterEnable(true);					// Enable/disable heater output
}

// Background Process, called from Task
void HeaterControl::process(){
		unsigned long timeNow = millis();
		hzDetermination(timeNow);
}

void HeaterControl::hzDetermination(unsigned long currentTime)				// Only accurate if called often enough to detect periodEnd time. IE must be called at least 1 / millisecond or better
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
void HeaterControl::heaterEnable(bool enable){						// Enable/disable heater output
	heaterEnabled = enable;
}

void HeaterControl::setPercentagePwr(unsigned char percentage){
	HeaterControl::powerPercentage = percentage;
	HeaterControl::powerCounter = (unsigned char)(((unsigned)percentage*FRAMESIZE)/100);	// Turn % into actual power on count
}
void HeaterControl::setRawPwr(unsigned char count){					// Called to set required raw power
	if (count > FRAMESIZE){
		HeaterControl::powerCounter = FRAMESIZE;
	}else
		HeaterControl::powerCounter = count;

	HeaterControl::powerPercentage = (HeaterControl::powerCounter*100)/FRAMESIZE;
}


// Port C ISR Driven
//Following methods are driven via Zero Crossing interrupt
void HeaterControl::checkACZeroCrossing(){								// Called from PortC ISR
		int portcValues	= PINC&(PCMSK_Z);
		int nz	= (portcValues&PCMSK_Z);								// New z value
		if(HeaterControl::prevZeroCrossingState & ~nz){					// new Z is low, previous was high, we have a zero crossing
			zeroCrossing();
		}
		HeaterControl::prevZeroCrossingState = nz;
}

#define LED_PIN   A4
void HeaterControl::zeroCrossing(){
	static int ledStatus = LOW;
	digitalWrite(HTR_PIN,LOW);
	zeroCount++;												// Used for Hz Calculation
	frameDetectionAndFiring();
	if(!(zeroCount%60)) ledStatus = ~ledStatus;
	digitalWrite(LED_PIN,ledStatus);

	/*
	++ledCntr %=100;	// divide freq by 100 for led visibility
	if(!ledCntr){
		ledV = !ledV;
		digitalWrite(HTR_PIN,ledV);
	}
	*/
}

void HeaterControl::fire(){
	if(heaterEnabled)
		digitalWrite(HTR_PIN,HIGH);
}

void HeaterControl::frameDetectionAndFiring(){
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
extern Display *myDisp;
extern DisplayText *displayElement;

// User Interface for Heater Control. Runs under System UI Thread
void HeaterControl::menuAction(int param){
	int i = 200;

	myDisp->tftScreen.background(0,255,0);
	displayElement->setBg(0, 255, 0);
	displayElement->setFg(255, 0, 0);
	while(!cancelled && i > 0){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time>nextDisplayTime){
			nextDisplayTime = time+500l;
			displayElement->setCol(0);  displayElement->setText((char *)dispBuff);

			sprintf(dispBuff, "Htr:%s ",HeaterControl::heaterEnabled?"On":"Off ");
			 displayElement->setRow(1);  displayElement->show();
			sprintf(dispBuff, "zHz: %d \nzCnt:%d ",zHz,zeroCount);
			 displayElement->setRow(2);  displayElement->show();
			sprintf(dispBuff, "Pwr%% : %d  ", HeaterControl::powerPercentage);
			 displayElement->setRow(4);  displayElement->show();
			sprintf(dispBuff, "Pwr Raw:%d  ", HeaterControl::powerCounter);
			 displayElement->setRow(5);  displayElement->show();
		}
		pause();
	}

	HeaterControl::heaterEnabled = false;
}

void HeaterControl::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR

//			Serial.print(F("HC Dir:"));Serial.println(direction);
	switch(type){
		case RotaryAction::ROTATE:
//						Serial.print(F("Heater Rotate"));
			switch(direction){
				case RSE::FW:
//						Serial.println(F("Heater Rotate FW"));
					if(HeaterControl::powerPercentage<100)
						HeaterControl::powerPercentage++;
					break;
				case RSE::RV:
//						Serial.println(F("Heater Rotate RV"));
					if(HeaterControl::powerPercentage > 0)
						HeaterControl::powerPercentage--;
					break;
				case RSE::NC:
//						Serial.println(F("Heater Rotate NC"));
					break;
			}
			break;
		case RotaryAction::SELECT:
			if(level == ButtonAction::BUTTONLOW){
//						Serial.println(F("Heater ButtonAction Low"));
				HeaterControl::heaterEnabled = !HeaterControl::heaterEnabled;
			}else{
//						Serial.println(F("Heater ButtonAction HIGH"));
			}
		break;
	}
}
