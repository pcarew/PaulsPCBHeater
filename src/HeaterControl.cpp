#include "arduino.h"
#include "pos/pos.h"
#include "HeaterControl.h"

	// Init statics
unsigned long HeaterControl::periodEnd		= 0;
long HeaterControl::zeroCount				= 0;
unsigned int HeaterControl::zHz				= 0;			// Holds the calculated AC frequency based upon AC Zero crossings. It should be 60Hz
bool HeaterControl::heaterEnabled			= false;
unsigned char HeaterControl::powerCounter	= 0;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
int HeaterControl::prevZeroCrossingState	= LOW;

	// Menu buffers
extern char dispBuff[];
//	const char *HeaterControl::fmt = "%s: %d.%dC  ";
const char *HeaterControl::fmt = "Heater Control";

HeaterControl::HeaterControl(){
}

void HeaterControl::process(){
		volatile unsigned long timeNow = millis();
		hzDetermination(timeNow);
}

void HeaterControl::setup(){
	HeaterControl::heaterEnable(true);					// Enable/disable heater output
}

void HeaterControl::heaterEnable(bool enable){						// Enable/disable heater output
	heaterEnabled = enable;
}
void HeaterControl::setPercentagePwr(unsigned char percentage){
	powerCounter = (unsigned char)(((unsigned)percentage*FRAMESIZE)/100);	// Turn % into actual power on count
}
void HeaterControl::setRawPwr(unsigned char count){					// Called to set required raw power
	if (count > FRAMESIZE){
		powerCounter = FRAMESIZE;
	}else
		powerCounter = count;
}

void HeaterControl::hzDetermination(unsigned long currentTime)				// Only accurate if called often enough to detect periodEnd time. IE must be called at least 1 / millisecond or better
{
	if(currentTime > periodEnd){
		periodEnd = currentTime + MEASUREMENT_PERIOD;
		zHz = zeroCount * HZ_MULTIPLIER;
		zeroCount = 0;
	}
}

// Used for Testing
extern bool cancelled;
extern Display *myDisp;
extern DisplayText *displayElement;

void HeaterControl::action(int param){
	int i = 200;

	cli();
	myDisp->tftScreen.background(0,255,0);
	sei();
	displayElement->setBg(0, 255, 0);
	displayElement->setFg(255, 0, 0);
	while(!cancelled && i > 0){
		sprintf(dispBuff, "Cnt: %d  \n",i--);
		displayElement->setCol(0); displayElement->setRow(2); displayElement->setText((char *)dispBuff); displayElement->show();
		pause();
	}

}




// Following methods are driven via Zero Crossing interrupt

void HeaterControl::checkACZeroCrossing(){
		int portcValues	= PINC&(PCMSK_Z);
		int nz	= (portcValues&PCMSK_Z);								// New z value

		if(HeaterControl::prevZeroCrossingState & ~nz){								// High to low
			zeroCount++;												// Used for Hz Calculation
			frameDetectionAndFiring();
		}
		HeaterControl::prevZeroCrossingState = nz;
}

void HeaterControl::fire(){
	if(heaterEnabled)
		digitalWrite(HTR_PIN,HIGH);
}

void HeaterControl::frameDetectionAndFiring(){
	static int frameFireCount = 0;
	static int skipCount = FRAMESIZE;								// Skip entire 1st frame
	static int cycleCount = 0;
//	Serial.println(F("Cycle ISR Called"));							// Dangerous for an ISR derived call
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
