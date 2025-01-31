/*
 * Profile.h
 *
 *  Created on: Mar 4, 2024
 *      Author: PaulCarew
 */

#ifndef PROFILE_H
#define PROFILE_H

#include "pos/pos.h"
#include "Extensions.h"
#include "Menu.h"

#define DEFGUARD 200
#define DEFSLOPE 3		// degrees/minute
#define DEFSOAK 5		// minutes

#define NUMBERPROFILES 5

/*
 *         +-----------    Guard Temp (Bottom not exceed temp)
 *        /
 *   (A) +---Soak Time---+ TargetTemp (Top temp). Soak time measured from time point A
 *      /
 *     /  <= Slope deg/minute (max rate)
 *    /
 * --+
 */
class Profile: Implements MenuAction{
private:
public:

	static Profile profiles[];
	static MenuItem profileMenuItems[];
	static int selectedProfile;

	const char *name;
	int topTargetTemp;
	int soakDuration;
	int bottomGuardTemp;
	int slope;

	Profile(int target,int guard, const char *name );
	Profile(int target,int guard,int soak,int slope, const char *name );

	void menuAction(int param);
//	void rotaryAction(const int type, int level, RSE::Dir direction, int param){};		// type is ROTATE or SELECT. 'level only used for SELECT

};

IMPORT Profile *profiles[NUMBERPROFILES];

#endif /* PROFILE_H */
