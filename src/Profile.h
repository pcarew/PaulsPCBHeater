/*
 * Profile.h
 *
 *  Created on: Mar 4, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILE_H
#define PROFILE_H

#define DEFGUARD 200
#define DEFSLOPE 3		// degrees/minute


/*
 *         +-----------    Guard Temp (Bottom not exceed temp)
 *        /
 *   (A) +---Soak Time---+ TargetTemp (Top temp). Soak time measured from time point A
 *      /
 *     /  <= Slope deg/minute (max rate)
 *    /
 * --+
 */
class Profile{
private:
public:
	enum PointState{
		NotActive	= 0,
		Adjusting	= 1,				// Warming or cooling towards target
		Soaking		= 2,					// Target attained, now soaking
		Complete	= 3				// Soaking complete
	};

	int topTargetTemp;
	int soakDuration;
	int bottomGuardTemp;
	int slope;

	PointState currantState;

	Profile(int target,int soak);
	Profile(int target,int guard,int soak,int slope);
};

#endif /* PROFILE_H */
