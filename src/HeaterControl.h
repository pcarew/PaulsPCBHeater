#ifndef HeaterControl_H
#define HeaterControl_H
#include "Menu.h"

#define FRAMESIZE ((int)15)
#define HTR_PIN   7				// D7
#define Z_PIN	3		//A3 PCMSK1 Bit#3 in PortC
#define PCMSK_Z (1<<Z_PIN)				// zPin
#define PCIR_PORTC 0x02					// PCIE1

// Zero Crossing Freq determination
#define MEASUREMENT_PERIOD (1500)									// period in milliseconds
#define HZ_MULTIPLIER	(1000.0/(double)MEASUREMENT_PERIOD)	// Convert cycle count in measurement period into Hz

extern volatile unsigned long time;
extern unsigned long nextDisplayTime;


class HeaterControl : Implements MenuAction{
private:
public:
	static unsigned long periodEnd;
	static unsigned int zeroCount;
	static unsigned int zHz;					// Holds the calculated AC frequency based upon AC Zero crossings. It should be 60Hz
	static bool heaterEnabled;
	static unsigned char powerCounter;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
	static unsigned char powerPercentage;			// The count of firings within Frame that equates with % as set by 'setPercentagePwr'
	static int prevZeroCrossingState;

	// Menu buffers
//	static const char *fmt;						// = "%s: %d.%dC  ";
	static const char *fmt;						// = "Heater Control";

	HeaterControl();
	~HeaterControl(){}
	static void process();
	static void setPercentagePwr(unsigned char percentage);		// Called by Temperature control to set required heater power
	static void updatePowerPercentage();						// Sets power percentage based on existing power count
	static void setRawPwr(unsigned char count);					// Called to set required raw power
	static void updatePowerCounter();							// Sets power count based on existing power percentage
	static void setup();

	//Used for UI
	void menuAction(int param);
	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT

	static void checkACZeroCrossing();
	static void zeroCrossing();
	static void setPower(int percent);
	static void heaterEnable(bool);					// Enable/disable heater output
	static void fire();
	static void frameDetectionAndFiring();
	static void hzDetermination(unsigned long currentTime);
};

#endif
