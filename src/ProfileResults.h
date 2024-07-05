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
	static void ProfileResults(unsigned long time,unsigned guard,unsigned guardCurrent, unsigned target,unsigned targetCurrent);
};

#endif /* PROFILERESULTS_H_ */
