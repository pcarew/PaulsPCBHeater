/*
 * LEDControl.h
 *
 *  Created on: Jun 27, 2024
 *      Author: PaulCarew
 */

#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_
#include "Arduino.h"

#define LED_PIN   A4

class LEDProfile{
private:
public:
	/*
	  Profile consists of:
	    A On+Off wavelength time T
	    A Percentage duration of ON out of T time
	    A flash count of the number of T Wavelengths in the cycle
	    A total size of cyle time. Any remaining time after the LED flashes is quiet time.

	   |+++++           +++++           +++++                     |
	   |+++++___________+++++___________+++++_____________________|
	    <D%-> 									(% of T)
	    <-Wavelength T->  						(in ms)
	    <----FlashCnt(n=3)------------------>	(int cnt)
	                                         <--QuietTime-------->
	    <----------------CycleTime--(in ms)---------------------->

	    	LED Cycle structure
	    	--------------------

	  	  	  	  	  +---------+
   |----------------->| Stopped |-----  CycleStartEv / Turn LED on, Start Duration (D) Timer, set flashCount (fc) to n
   | 	  	  	  	  +---------+    |
   | 	  	  	  	                 |
   | 	  	--------->+---------+<----
   | 	  	|  	 ---->| LEDON   |-----  Duration Complete Ev / Turn LED off, Start (T-D) timer, dec fc
   | 	  	|  	 |    +---------+     |
   | 	  	|  	 |                    |
   | 	  	|  	 |                    |
   | 	  	|  	 |    +---------+<-----
   | 	  	|  	 -----| LEDOFF  |------  LedQuietEv (T-D expiration and no more flashes) / Start quiet Timer
   | 	  	|   MoreEv+---------+     |
   | 	  	|  	/ LED on              |
   |        |                         |
   | 	  	----------+---------+     |
   |   CycleStEv      |         |<-----
   |   /LEDOn,fc=n etc| LEDQUIET|
   -------------------|         |
      CycleOffEv	  +---------+


  	  	   LED cycle finite state machine
  	  	   ------------------------------

	 */
	int wavelengthT;		// Wavelength time in millis
	unsigned char duty;		// Duty cycle %
	unsigned char flashCnt;	// Number of LED flash cycles
	int cycleTime;			// Overall time  in millis before cycle repeats
};

class LEDController{
private:
public:
	enum  LEDMode{
		SelfTest		= 0,
		Off				= 1,
		ZeroCrossing	= 2,
		HeaterOn		= 3,
		AmbientWarning	= 4,
		AmbientDanger	= 5
	};

	enum LEDState {
		LEDSTOPPED =0,	// No LED activity
		LEDON	= 1,	// Led is on, waiting for Duration expiration
		LEDOFF	= 2,	// Led is Off, waiting for T expiration
		LEDQUIET= 3		// Led is off, waiting for Cycle Reset
	};

	enum LEDEvent{
		CycleStartEv	= 0,			// Start of New LED Flash Cycle
		LEDOnComplete	= 1,			// End of LED on Time
		MoreEv			= 2,			// End of LED Off Time with more flashes to be performed
		LedQuietEv		= 4,			// End of LED Off Time with *No* more flashes to be performed
		CycleOffEv		= 5				// End of  quiet time and Mode is now 'Off'
	};

	static const LEDProfile ledProfiles[];
	static LEDMode ledMode;
	static LEDState ledCycleState;

	static void setup();
	static void update();
	static void displayCycle();

	static void ledSetMode(LEDMode mode);
	static LEDEvent detectEvent();
};

#endif /* LEDCONTROLLER_H_ */
