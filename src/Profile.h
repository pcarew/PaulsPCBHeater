/*
 * Profile.h
 *
 *  Created on: Mar 4, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILE_H
#define PROFILE_H

#define MAXPOINTS 8

struct Point{
	double temp;
	double duration;
	double slope;
//	double startingOffset;

};

class Profile{
private:
public:
	enum PointState{
		InSlope = 1,
		Attained =2,
		Holding = 3
	};


	int numberPoints;

	Point points[MAXPOINTS];
	char currentPoint;
	PointState currantState;
};

#endif /* PROFILE_H */
