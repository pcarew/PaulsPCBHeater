/*
 * ProfileResults.h
 *
 *  Created on: Jul 5, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILERESULTS_H_
#define PROFILERESULTS_H_

class ProfileResults {
public:
	ProfileResults();
	virtual ~ProfileResults();
	static void tempDataPacket(unsigned long time,unsigned guard,unsigned guardCurrent, unsigned target,unsigned targetCurrent);
	static void startNewProfile(unsigned guard, unsigned target);

};

#endif /* PROFILERESULTS_H_ */
