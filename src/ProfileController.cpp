/*
 * ProfileController.cpp
 *
 *  Created on: May 2, 2024
 *      Author: PaulCarew
 */

#include "pos/pos.h"
#include "ProfileController.h"
#include "Profile.h"
#include "Menu.h"
#include "Button.h"
#include "SystemData.h"
#include "ProfileResults.h"

//#include "Ram.h"


ProfileController::ProfileController() {
}

ProfileController::~ProfileController() {
}



// Used for UI
char ProfileController::activePage = 0;
const char *ProfileController::profileNameFmt = "%sT%3d G%3d";
//Menu* ProfileController::localMenu		= NULL;
//Menu* ProfileController::profileMenu	= NULL;
bool ProfileController::profileRunning = false;

// Profile control
Profile *ProfileController::activeProfile = NULL;
//ProfileController::ProfileState ProfileController::currantState = ProfileController::NotActive;
unsigned long ProfileController::profileTime = 0;
int ProfileController::manualProfileTargetTemp = 50;
int ProfileController::manualProfileGuardTemp = 100;

#define MainPg 	  (ProfileControlId	+0)
#define ManTempPg (ProfileControlId	+1)
#define ProfSelPg (ProfileControlId	+2)
#define StrtStpPg (ProfileControlId	+3)
#define RestartPg (ProfileControlId	+4)
#define HtrCtlPg (ProfileControlId	+5)

#define	NUMBERITEMS 5
MenuItem ProfileController::localMenuItems[NUMBERITEMS] {
		{"ManTemp",		&profileController, ManTempPg, false},
		{"ProfileSel",	&profileController, ProfSelPg, false},
		{"Start/Stop",	&profileController, StrtStpPg, false},
		{"Restart",		&profileController, RestartPg, false},
		{"Htr Enb/Dis",	&profileController, HtrCtlPg, false}
};

Menu ProfileController::localMenu( (MenuItem*)ProfileController::localMenuItems,	NUMBERITEMS,	(RotarySelector*)NULL,&systemDisplay,2,1); //  starting at Row 2, col 1
Menu ProfileController::profileMenu( (MenuItem*)Profile::profileMenuItems,			NUMBERPROFILES,	(RotarySelector*)NULL,&systemDisplay,2,0); //  starting at Row 2, col 1

// User Interface for Profile Controller. Runs under System UI Thread
void mainPage();
void manPage();
void profileSelectionPage();
void startStopProfilePage();
void restartProfilePage();
void heaterContolPage();
	char fmt[] = "  T%3d G%3d ";
void ProfileController::menuAction(volatile int param){

	ProfileController::activePage = param;
	systemDisplay.clear(0,255,0);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	ProfileController::localMenu.currentMenuItemId = -1;
	displayElement.setCol(0);  displayElement.setText((char *)dispBuff);
	time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
	switch(ProfileController::activePage){
		case MainPg:							// Top level page, display sub menu
			mainPage();
			cancelled = false;
			break;
		case ManTempPg:
			manPage();
			cancelled = false;
			break;
		case ProfSelPg:
			profileSelectionPage();
			cancelled = false;
			break;
		case StrtStpPg:
			startStopProfilePage();
			cancelled = false;
			break;
		case RestartPg:
			restartProfilePage();
			cancelled = false;
			break;
		case HtrCtlPg:
			heaterContolPage();
			cancelled = false;
			break;
		default:
			break;
	}
	return;
}

void ProfileController::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR
	static ManualUpdate manUpdate = ManualUpdate::Target;

	switch(ProfileController::activePage){
		case ProfileControlId:							// Top level page, display sub menu
			ProfileController::localMenu.rotaryAction(type, level, direction, param);					// Let the local menu handle the Rotary
			break;
		case ManTempPg:
			if(manUpdate == ManualUpdate::Target){
				if(ProfileController::handleRotary(type, level, direction, &ProfileController::manualProfileTargetTemp) == true) manUpdate = ManualUpdate::Guard;
				if(ProfileController::manualProfileTargetTemp < 0 )
					ProfileController::manualProfileTargetTemp = 0 ;
			}else{
				if(ProfileController::handleRotary(type, level, direction, &ProfileController::manualProfileGuardTemp) == true) manUpdate = ManualUpdate::Target;
				if(ProfileController::manualProfileGuardTemp < 0 )
					ProfileController::manualProfileGuardTemp = 0 ;
			}
			break;
		case ProfSelPg:
			ProfileController::profileMenu.rotaryAction(type, level, direction, param);					// Let the local menu handle the Rotary
			break;
		case StrtStpPg:
			break;
		case RestartPg:
			break;
		default:
			break;
	}
}

bool ProfileController::handleRotary(const int type, int level, RSE::Dir direction, int *counter){		// type is ROTATE or SELECT Called from PortD ISR
	switch(type){
		case RotaryAction::ROTATE:
			if(counter == NULL) return false;								// Make sure we have something to twiddle
			switch(direction){
				case RSE::FW:
						if(counter != NULL) (*counter)++;
					break;
				case RSE::RV:
						if(counter != NULL) (*counter)--;
					break;
				case RSE::NC:
					break;
			}
			break;
		case RotaryAction::SELECT:
			if(level == ButtonAction::BUTTONLOW){
				return true;
		}else{
//					Serial.println(F("Temperature ButtonAction HIGH"));
			}
		break;
	}
	return false;
}

// Called by Task thread
#define REPORTINTERVAL 1000l			// 1 Second
void ProfileController::update(){

	static unsigned long reportTime = 0;
	unsigned long time = millis();
	if(time>reportTime){
		reportTime = time+REPORTINTERVAL;
		ProfileController::profileTime += REPORTINTERVAL;
		if(ProfileController::profileRunning == true ){	// Predefined Profile
			ProfileResults::tempDataPacket((ProfileController::profileTime/REPORTINTERVAL),				// Convert to seconds
				ProfileController::activeProfile->bottomGuardTemp, (unsigned)TemperatureMonitoring::brdBot.getTemperature(),
				ProfileController::activeProfile->topTargetTemp, (unsigned)TemperatureMonitoring::brdTop.getTemperature()
				);
		} else {	// Manual Profile
			ProfileResults::tempDataPacket((ProfileController::profileTime/REPORTINTERVAL),				// Convert to seconds
				ProfileController::manualProfileGuardTemp, (unsigned)TemperatureMonitoring::brdBot.getTemperature(),
				ProfileController::manualProfileTargetTemp, (unsigned)TemperatureMonitoring::brdTop.getTemperature()
				);

		}
	}

}


void mainPage(){
	unsigned long displayTime2 = 0;;
	systemDisplay.clear();
	ProfileController::activePage = ProfileControlId;
	displayElement.show();
	while(!cancelled ){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)

		if(time>nextDisplayTime){								// Menu subsystem
			nextDisplayTime = time+250l;
			ProfileController::localMenu.menuInvoke();						// **** This is a recursive call ***
			ProfileController::activePage = ProfileControlId;				// This may have changed via menu handling
		}

		if(time>displayTime2){									// Status Display
			displayTime2 = time+500l;

			if(ProfileController::profileRunning == true){		// If Profile is *running* indicate by changing FG+BG colours
				displayElement.setBg(0, 255, 0);
				displayElement.setFg(255, 0, 0);
			}else{
				displayElement.setBg(0, 0, 255);
				displayElement.setFg(255, 255, 255);

			}

			if(ProfileController::activeProfile == NULL){		// Display either manual temp or profile temp targets
				sprintf(dispBuff,ProfileController::profileNameFmt, "M:", (ProfileController::manualProfileTargetTemp), (ProfileController::manualProfileGuardTemp));
			}else{
				sprintf(dispBuff,ProfileController::profileNameFmt, "P:", ProfileController::activeProfile->topTargetTemp, ProfileController::activeProfile->bottomGuardTemp);
			}
			displayElement.setText((char *)dispBuff);
			displayElement.setCol(0); displayElement.setRow(0); displayElement.show();

			displayElement.setBg(0, 0, 255);					// Reset for rest of status display

			if(HeaterController::heaterEnabled)
				displayElement.setFg(255, 0, 0);
			else
				displayElement.setFg(255, 255, 255);
			sprintf(dispBuff, fmt, (unsigned)TemperatureMonitoring::brdTop.getTemperature(), (unsigned)TemperatureMonitoring::brdBot.getTemperature() );
			displayElement.setRow(1); displayElement.show();
			displayElement.setFg(255, 255, 255);
		}

		pause();
	}
}

void manPage(){
	int lastTt =0, lastGt=0;
	systemDisplay.clear();
	sprintf_P(dispBuff, PSTR("ManualTemp"));
	displayElement.setCol(0);
	displayElement.setRow(0);
	displayElement.setText((char *)dispBuff);
	displayElement.show();
//	ProfileController::currantState = ProfileController::NotActive;
	ProfileController::activeProfile = NULL;

	nextDisplayTime = 0;					// Force first display to be immediate
	lastTt =-1; lastGt=-1;

	while(!cancelled ){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyone (and ourselves)
		if(time>nextDisplayTime){
			nextDisplayTime = time+120l;	// Check once per period, but only update if needed
			if( ProfileController::manualProfileTargetTemp != lastTt || ProfileController::manualProfileGuardTemp != lastGt){
				lastTt = ProfileController::manualProfileTargetTemp;
				lastGt = ProfileController::manualProfileGuardTemp;
				sprintf(dispBuff, fmt,  ProfileController::manualProfileTargetTemp, ProfileController::manualProfileGuardTemp);
				displayElement.setRow(1); displayElement.show();
//								Serial.println(dispBuff);delay(10);
			}
		}

		pause();
	}
	if(TemperatureController::getTargetTemperature() != ProfileController::manualProfileTargetTemp){	// Update Temperature Controller if needed
		TemperatureController::setTemperature(ProfileController::manualProfileTargetTemp,ProfileController::manualProfileGuardTemp,0);
	}
	ProfileController::profileTime = 0;			// Manual Profile
	ProfileResults::startNewProfile( ProfileController::manualProfileGuardTemp, ProfileController::manualProfileTargetTemp);
}

void profileSelectionPage(){
	unsigned long displayTime2 = 0;;
	systemDisplay.clear();
//	Serial.print(F("Profile Selection here. Pg:"));Serial.println((int)ProfileController::activePage);delay(20);

	ProfileController::profileMenu.currentMenuItemId = -1;
	while(!cancelled ){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time>nextDisplayTime){
			nextDisplayTime = time+250l;
			ProfileController::profileMenu.menuInvoke();						// **** This is a recursive call ***
			if(Profile::selectedProfile != -1){
				ProfileController::activeProfile = &Profile::profiles[Profile::selectedProfile];
					// ProfileController task thread will make use of the active Profile
			}else{
				ProfileController::activeProfile = NULL;
			}
		}
		if(time>displayTime2){
			displayTime2 = time+500l;
				sprintf(dispBuff, "Profile Sel. %d",(ProfileController::activeProfile!=NULL?ProfileController::activeProfile->topTargetTemp:0)); displayElement.setRow(0); displayElement.setCol(0);
				displayElement.setText((char *)dispBuff);
				displayElement.show();
		}
		pause();
	}
}

void startStopProfilePage(){
	systemDisplay.clear();
//			sprintf(dispBuff, "Start / Stop"); displayElement.setRow(0);  displayElement.show();
//				Serial.print(F("Start Stop here. Pg:"));Serial.println((int)ProfileController::activePage);delay(10);

	if(ProfileController::profileRunning == false && ProfileController::activeProfile != NULL){
		ProfileController::profileRunning = true;
//		ProfileController::currantState = ProfileController::Adjusting;
		ProfileController::profileTime = 0;
		TemperatureController::setTemperature(
				ProfileController::activeProfile->topTargetTemp,
				ProfileController::activeProfile->bottomGuardTemp,
				ProfileController::activeProfile->soakDuration
			);
		ProfileResults::startNewProfile( ProfileController::activeProfile->bottomGuardTemp, ProfileController::activeProfile->topTargetTemp);

	}else{
		ProfileController::profileRunning = false;
//		ProfileController::currantState = ProfileController::NotActive;
		TemperatureController::setTemperature( NULL,NULL,NULL);
	}
}

void restartProfilePage(){
	if(ProfileController::activeProfile != NULL){
		ProfileController::profileRunning = true;
		TemperatureController::setTemperature(
				ProfileController::activeProfile->topTargetTemp,
				ProfileController::activeProfile->bottomGuardTemp,
				ProfileController::activeProfile->soakDuration
			);
		ProfileResults::startNewProfile( ProfileController::activeProfile->bottomGuardTemp, ProfileController::activeProfile->topTargetTemp);
	}else{
		ProfileResults::startNewProfile( ProfileController::manualProfileGuardTemp, ProfileController::manualProfileTargetTemp);
	}
	ProfileController::profileTime = 0;
}

void heaterContolPage(){
	systemDisplay.clear();
	sprintf(dispBuff, "HtrCtlPg"); displayElement.setRow(0);  displayElement.show();
	HeaterController::heaterEnable(!HeaterController::heaterEnabled);
}
