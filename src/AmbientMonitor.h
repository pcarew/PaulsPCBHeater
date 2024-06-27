/*
 * AmbientMonitor.h
 *
 *  Created on: Jun 26, 2024
 *      Author: PaulCarew
 */

#ifndef AMBIENTMONITOR_H_
#define AMBIENTMONITOR_H_

#define AM_MEASUREMENT_PERIOD 500		// Every 1/2 second

#define AMBIENTWARNING	40
#define AMBIENTDANGER	60

class AmbientMonitor {
public:
	AmbientMonitor();
	virtual ~AmbientMonitor();
	static void update();				// Called from Tasking
};

#endif /* AMBIENTMONITOR_H_ */
