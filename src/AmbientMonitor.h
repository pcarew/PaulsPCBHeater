/*
 * AmbientMonitor.h
 *
 *  Created on: Jun 26, 2024
 *      Author: PaulCarew
 */

#ifndef AMBIENTMONITOR_H_
#define AMBIENTMONITOR_H_

#define AM_MEASUREMENT_PERIOD	5000l		// Every 5 seconds
#define AM_DELAY_START			5000l		// Delayed start for  Ambient monitoring

#define AMBIENTWARNING	40					// Warning starts at 40 degrees C
#define AMBIENTDANGER	60					// Danger starts at 40 degrees C

class AmbientMonitor {
public:
	AmbientMonitor();
	virtual ~AmbientMonitor();
	static void update();				// Called from Tasking
};

#endif /* AMBIENTMONITOR_H_ */
