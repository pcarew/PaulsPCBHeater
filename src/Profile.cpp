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

Profile Profile::profiles[NUMBERPROFILES]  {				// @suppress("Symbol is not resolved")
				//Target, Guard, Soak, Slope, Name
		Profile(40,60, 					"T 40 G 60"),
		Profile(60,90,DEFSOAK,DEFSLOPE,	"T 60 G 90"),
		Profile(100,150, 				"T100 G150"),
		Profile(150,200,				"T150 G200"),
		Profile(195,200,				"T195 G200")
};

MenuItem Profile::profileMenuItems [NUMBERPROFILES] {	// @suppress("Symbol is not resolved")
	{profiles[0].name,&profiles[0],0,false},
	{profiles[1].name,&profiles[1],1,false},
	{profiles[2].name,&profiles[2],2,false},
	{profiles[3].name,&profiles[3],3,false},
	{profiles[4].name,&profiles[4],4,false}

//	{profiles[0].name,&profiles[0],0,false},
//	{profiles[1].name,&profiles[1],1,false},
//	{profiles[2].name,&profiles[2],2,false},
//	{profiles[3].name,&profiles[3],3,false}
};

void Profile::menuAction(int param){
	Profile::selectedProfile = param;
//	Serial.print(F("Selected profile is:"));Serial.println(param);
}

/* Using standard menu rotary handling
void Profile::rotaryAction(const int type, int level, RSE::Dir direction, int param){ // type is ROTATE or SELECT. 'level only used for SELECT

}
*/
