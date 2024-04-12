#ifndef HeaterControl_H
#define HeaterControl_H
#include "Menu.h"

#define FRAMESIZE ((int)15)
#define HTR_PIN   7				// D7
#define Z_PIN	3		//A3 PCMSK1 Bit#3 in PortC
#define PCMSK_Z (1<<Z_PIN)				// zPin

// Zero Crossing Freq determination
#define MEASUREMENT_PERIOD (1500)									// period in milliseconds
#define HZ_MULTIPLIER	(1000.0/(double)MEASUREMENT_PERIOD)	// Convert cycle count in measurement period into Hz

class HeaterControl : Implements MenuAction{
private:
public:
	static unsigned long periodEnd;
	static long zeroCount;
	static unsigned int zHz;					// Holds the calculated AC frequency based upon AC Zero crossings. It should be 60Hz
	static bool heaterEnabled;
	static unsigned char powerCounter;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
	static int prevZeroCrossingState;

	// Menu buffers
//	static const char *fmt;						// = "%s: %d.%dC  ";
	static const char *fmt;						// = "Heater Control";

	HeaterControl();
	~HeaterControl(){}
	static void process();
	static void setPercentagePwr(unsigned char percentage);		// Called by Temperature control to set required heater power
	static void setRawPwr(unsigned char count);		// Called to set required raw power
	static void setup();

	//Used for Testing
	void action(int param);

	static void checkACZeroCrossing();
	static void setPower(int percent);
	static void heaterEnable(bool);					// Enable/disable heater output
	static void fire();
	static void frameDetectionAndFiring();
	static void hzDetermination(unsigned long currentTime);
};

#endif
