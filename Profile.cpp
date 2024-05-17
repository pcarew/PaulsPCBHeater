/*
 * Profile.cpp
 *
 *  Created on: May 17, 2024
 *      Author: PaulCarew
 */

#include "Profile.h"

Profile::Profile(int target,int soak): Profile(target,DEFGUARD,soak,DEFSLOPE){}
Profile::Profile(int target,int guard,int soak,int slope){
	this->topTargetTemp		= target;
	this->bottomGuardTemp	= guard;
	this->soakDuration		= soak;
	this->slope				= slope;
	this->currantState		= NotActive;
}

Profile profiles[] = {
		new Profile(50,5),
		new Profile(100,5),
		new Profile(200,5)
};


