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

#define NUMRESULTS 16

#define LOG10_1_3	((double)0.11394)					// Log10(1.3)
#define LOG10_2		((double)0.30103)					// Log10(2)
#define LOG10_4		((double)0.60201)					// Log10(4)
#define LG1_3		((double)0.26224)					// Ln(1.3)
#define LG2			((double)0.69315)					// Ln(2)
#define LG4			((double)1.3863)					// Ln(4)

//#define BUCKET(n)	((n<4096)?((int)(log(n)/LG4)):6)	// Which array bucket to accumulate value

																								// Which array bucket to accumulate value.
																								// If time is < minute use bucket 0
																								// If Time is over (1.3^15) minutes (3071 seconds) use the top bucket (1)
																								// If it's in between, use log (seconds/60)/log(1.3) to determine bucket#
//#define BUCKET(seconds)( (int)(((unsigned)seconds<60)?0: ((unsigned)seconds > 3071)?15: ((log((double)seconds)/LG1_3)+0.9)))
//#define BUCKET(seconds)( (int)(((unsigned)seconds<60)?0: ((unsigned)seconds > 3071)?15: ((log((double)seconds)/LG1_3)+0.9)))

//#define BUCKET(seconds)	( (seconds<60)?0: ((seconds<15360?((int)(log((seconds/60))/LG2)) : 7)))

#define BucketAVG(bucketContents,value)	((int)(((double)bucketContents+(double)value)/2.0))

#define PROFILEGUARD  0
#define PROFILETARGET 1


#define GRAPHUPDATETIME 1000l

class ProfileGrapher: Implements MenuAction {
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
