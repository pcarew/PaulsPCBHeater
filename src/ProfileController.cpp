/*
 * ProfileController.cpp
 *
 *  Created on: May 2, 2024
 *      Author: PaulCarew
 */

#include "pos/pos.h"
#include "ProfileController.h"
#include "Menu.h"
#include "Ram.h"
#include "SystemData.h"
#include <util/atomic.h>

#include "Button.h"

ProfileController::ProfileController() {
}

ProfileController::~ProfileController() {
}



// Used for UI
const char *ProfileController::fmt = "Profile Control";
int ProfileController::targetTemp = 0;

#define MainPg 	  (ProfileControlId	+0)
#define ManTempPg (ProfileControlId	+1)
#define ProfSelPg (ProfileControlId	+2)
#define StrtStpPg (ProfileControlId	+3)
#define RestartPg (ProfileControlId	+4)
#define HtrCtlPg (ProfileControlId	+5)

#define	NUMBERITEMS 5
MenuItem *ProfileController::localMenuItems = new MenuItem [NUMBERITEMS] {
		{"ManTemp",		&profileController, ManTempPg, false},
		{"ProfileSel",	&profileController, ProfSelPg, false},
		{"Start/Stop",	&profileController, StrtStpPg, false},
		{"Restart",		&profileController, RestartPg, false},
		{"Htr Enb/Dis",	&profileController, HtrCtlPg, false}
};

Menu* ProfileController::localMenu = new Menu( (MenuItem*)ProfileController::localMenuItems, NUMBERITEMS, (RotarySelector*)NULL,&systemDisplay,2,1); //  starting at Row 2, col 1
char ProfileController::activePage = 0;

// User Interface for Profile Controller. Runs under System UI Thread
void ProfileController::menuAction(int param){
	unsigned long displayTime2 = 0;;
	int lastTt =0, lastAt=0;
	ProfileController::activePage = param;
	systemDisplay.clear(0,255,0);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	ProfileController::localMenu->currentMenuItemId = -1;
	displayElement.setCol(0);  displayElement.setText((char *)dispBuff);
	time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
	switch(ProfileController::activePage){
		case MainPg:							// Top level page, display sub menu
			ProfileController::activePage = ProfileControlId;
			displayElement.show();
			while(!cancelled ){
				time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
				if(time>nextDisplayTime){
					nextDisplayTime = time+250l;
					ProfileController::localMenu->menuInvoke();						// **** This is a recursive call ***
					ProfileController::activePage = ProfileControlId;				// This may have changed
					ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
						sprintf(dispBuff, "Profile Cntl");
					}
					displayElement.setText((char *)dispBuff);						// MenuInvoke may have changed these
					displayElement.setCol(0);
					displayElement.setRow(0);
					displayElement.show();
				}

				if(time>displayTime2){
					displayTime2 = time+1000l;
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
							sprintf(dispBuff, "TT:%d AT:%d ", ProfileController::targetTemp,(int)TemperatureMonitoring::brdBot.getTemperature());
						}
						displayElement.setRow(1);
						displayElement.show();
				}
				pause();
			}
			cancelled = false;
			break;
		case ManTempPg:
			sprintf(dispBuff, "ManualTemp");
			displayElement.setCol(0);
			displayElement.setRow(0);
			displayElement.setText((char *)dispBuff);
			displayElement.show();
			while(!cancelled ){
				if(TemperatureController::getTemperature() != ProfileController::targetTemp){	// Update Temperature Controller if needed
					TemperatureController::setTemperature(ProfileController::targetTemp, 0);
				}

				time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
				if(time>nextDisplayTime){
					nextDisplayTime = time+100l;	// Check once per period, but only update if needed
					if( ProfileController::targetTemp != lastTt || TemperatureMonitoring::brdBot.getTemperature() != lastAt){
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
							sprintf(dispBuff, "TT:%d AT:%d ", ProfileController::targetTemp,(int)TemperatureMonitoring::brdBot.getTemperature());
							lastTt = ProfileController::targetTemp;
							lastAt = TemperatureMonitoring::brdBot.getTemperature();
							displayElement.setRow(1); displayElement.show();
//								Serial.println(dispBuff);
						}
					}
				}

				/*
				if(time>displayTime2){ // Let serial monitor know every so often
					displayTime2 = time+2000l;
					Serial.print(F("Man TempPg here. Pg:"));Serial.print((int)ProfileController::activePage);delay(10);
					Serial.print(F("Ram free"));Serial.println(ramApp.freeRam());delay(10);
				}
				*/
				pause();
			}
			cancelled = false;
			break;
		case ProfSelPg:
			sprintf(dispBuff, "Profile Sel."); displayElement.setRow(0);  displayElement.show();
			while(!cancelled ){
				Serial.print(F("Profile Selection here. Pg:"));Serial.println((int)ProfileController::activePage);delay(10);
				pause();
			}
			cancelled = false;
			break;
		case StrtStpPg:
			sprintf(dispBuff, "Start / Stop"); displayElement.setRow(0);  displayElement.show();
			while(!cancelled ){
				Serial.print(F("Start Stop here. Pg:"));Serial.println((int)ProfileController::activePage);delay(10);
				pause();
			}
			cancelled = false;
			break;
		case RestartPg:
			sprintf(dispBuff, "ReStart"); displayElement.setRow(0);  displayElement.show();
			while(!cancelled ){
				Serial.print(F("Restart here. Pg:"));Serial.println((int)ProfileController::activePage);delay(10);
				pause();
			}
			cancelled = false;
			break;
		case HtrCtlPg:
			sprintf(dispBuff, "HtrCtlPg"); displayElement.setRow(0);  displayElement.show();
			HeaterController::heaterEnable(!HeaterController::heaterEnabled);
			cancelled = false;
			break;
		default:
			break;
	}
	return;
}

void ProfileController::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR

	switch(ProfileController::activePage){
		case ProfileControlId:							// Top level page, display sub menu
			ProfileController::localMenu->rotaryAction(type, level, direction, param);					// Let the local menu handle the Rotary
			break;
		case ManTempPg:
			ProfileController::handleRotary(type, level, direction, &ProfileController::targetTemp);
			if(ProfileController::targetTemp < 0 )
				ProfileController::targetTemp = 0 ;
			break;
		case ProfSelPg:
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
							(*counter)++;
						break;
					case RSE::RV:
							(*counter)--;
						break;
					case RSE::NC:
						break;
				}
				break;
			case RotaryAction::SELECT:
				if(level == ButtonAction::BUTTONLOW){
					return true;
				}else{
//						Serial.println(F("Temperature ButtonAction HIGH"));
				}
			break;
		}
		return false;
	}
