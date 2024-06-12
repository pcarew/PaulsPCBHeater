/*
 * Profile.cpp
 *
 *  Created on: May 17, 2024
 *      Author: PaulCarew
 */

#include "Profile.h"

int Profile::selectedProfile = -1;

Profile::Profile(int target,int guard,const char *name): Profile(target,guard,DEFSOAK,DEFSLOPE, name){}
Profile::Profile(int target,int guard,int soak,int slope, const char *name){

	this->topTargetTemp		= target;
	this->bottomGuardTemp	= guard;
	this->soakDuration		= soak;
	this->slope				= slope;

	this->name				= name;
}

Profile *Profile::profiles = new Profile[NUMBERPROFILES]  {
		Profile(50,60,DEFSOAK,DEFSLOPE,	"T50  G60"),
		Profile(100,150, 				"T100 G150"),
		Profile(200,200,				"T200 G200")
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
