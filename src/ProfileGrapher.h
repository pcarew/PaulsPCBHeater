/*
 * Graph.h
 *
 *  Created on: Jul 2, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILEGRAPHER_H_
#define PROFILEGRAPHER_H_
#include <Arduino.h>
#include "Menu.h"

#define NUMRESULTS 7

#define LOG10_4		((double)0.60201)					// Log10(4)
#define LG4			((double)1.3863)					// Ln (4)
#define BUCKET(n)	((n<4096)?((int)(log(n)/LG4)):6)	// Which array bucket to accumulate value

#define PROFILEGUARD  0
#define PROFILETARGET 1

#define BucketAVG(bucketCapacity,bucketContents,value)	((unsigned char)(((double)bucketContents*(double)(bucketCapacity-1)+(double)value)/((double)bucketCapacity)))

class ProfileGrapher: Implements MenuAction{
public:

//	const static int height;
//	const static int width;

	/*
	 * Logarithmic results array
	 * Each 'bucket' is 4x the previous, with the first bucket being 1 second.
	 *
	 */
	static unsigned char results[2][NUMRESULTS];		// 1st row is Guard results, 2nd is target results
	static unsigned char currentBucket;					// The bucket being accumulated
	static unsigned char guard;
	static unsigned char target;

	ProfileGrapher(){}
	virtual ~ProfileGrapher(){}

//	void update();
	static void tempDataPacket(unsigned long time,unsigned guardCurrent, unsigned targetCurrent);
	static void startNewProfile(unsigned guard, unsigned target);

	//Used for UI
	void menuAction(int param);
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param);		// type is ROTATE or SELECT
//	bool handleRotary(const int type, int level, RSE::Dir direction, int *counter);	// Used for processing rotary action for local pages

	static void drawAxis();
};

#endif /* PROFILEGRAPHER_H_ */
