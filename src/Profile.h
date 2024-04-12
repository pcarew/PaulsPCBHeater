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
	double startOffset;

};

class Profile{
private:
public:
	int numberPoints;

	Point points[MAXPOINTS];
};

#endif /* PROFILE_H */
