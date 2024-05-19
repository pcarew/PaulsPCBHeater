/*
 * Profile.cpp
 *
 *  Created on: May 17, 2024
 *      Author: PaulCarew
 */

#include "Profile.h"

int Profile::selectedProfile = -1;

Profile::Profile(int target,int soak, const char *name): Profile(target,DEFGUARD,soak,DEFSLOPE, name){}
Profile::Profile(int target,int guard,int soak,int slope, const char *name){

	this->topTargetTemp		= target;
	this->bottomGuardTemp	= guard;
	this->soakDuration		= soak;
	this->slope				= slope;
	this->currantState		= NotActive;

	this->name				= name;
}

Profile *Profile::profiles = new Profile[NUMBERPROFILES]  {
		Profile(50,5,"T:50 S:5"),
		Profile(100,5,"T:100 S:5"),
		Profile(200,5,"T:200 S:5")
};

MenuItem *Profile::profileMenuItems = new MenuItem [NUMBERPROFILES] {
	{profiles[0].name,&profiles[0],0,false},
	{profiles[1].name,&profiles[1],1,false},
	{profiles[2].name,&profiles[2],2,false}
};

void Profile::menuAction(int param){
	Profile::selectedProfile = param;
	Serial.print(F("Selected profile is:"));Serial.println(param);
}

/*
void Profile::rotaryAction(const int type, int level, RSE::Dir direction, int param){ // type is ROTATE or SELECT. 'level only used for SELECT

}
*/
