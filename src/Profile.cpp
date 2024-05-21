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

	this->name				= name;
}

Profile *Profile::profiles = new Profile[NUMBERPROFILES]  {
		Profile(50,45,5,DEFSLOPE,	"T50  G45  S5"),
		Profile(100,5,				"T100 G200 S5"),
		Profile(200,5,				"T200 G200 S5")
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
