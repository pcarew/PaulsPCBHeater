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

ProfileController::ProfileController() {
}

ProfileController::~ProfileController() {
}



// Used for UI
const char *ProfileController::fmt = "Profile Control";
unsigned char ProfileController::targetTemp = 44;
#define ManTempPg (ProfileControlId	+1)
#define ProfSelPg (ProfileControlId	+2)
#define StrtStpPg (ProfileControlId	+3)
#define RestartPg (ProfileControlId	+4)

#define	NUMBERITEMS 4
MenuItem *ProfileController::localMenuItems = new MenuItem[NUMBERITEMS] {
		{"ManTemp",		&profileController, ManTempPg, false},
		{"ProfileSel",	&profileController, ProfSelPg, false},
		{"Start/Stop",	&profileController, StrtStpPg, false},
		{"Restart",		&profileController, RestartPg, false}
};

Menu* ProfileController::localMenu = new Menu( (MenuItem*)ProfileController::localMenuItems, NUMBERITEMS, (RotarySelector*)NULL,&systemDisplay,2,1); //  starting at Row 2, col 1
char ProfileController::activePage = 0;

IMPORT Ram ramApp;

// User Interface for Profile Controller. Runs under System UI Thread
void ProfileController::menuAction(int param){

	ProfileController::activePage = param;
	systemDisplay.tftScreen.background(0,255,0);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	ProfileController::localMenu->currentMenuItemId = -1;
	time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
	switch(ProfileController::activePage){
		case ProfileControlId:							// Top level page, display sub menu
			while(!cancelled ){
			//			Serial.println(F("Invoking local menu"));
				ProfileController::localMenu->menuInvoke();						// **** This is a recursive call ***
				ProfileController::activePage = ProfileControlId;
				if(time>nextDisplayTime){
					nextDisplayTime = time+500l;
					displayElement.setCol(0);  displayElement.setText((char *)dispBuff);

					sprintf(dispBuff, "ProfContr");
						displayElement.setRow(0);  displayElement.show();
					sprintf(dispBuff, "Tmp: %d  ", ProfileController::targetTemp);
						displayElement.setRow(1);  displayElement.show();
				}
				pause();
			}
			break;
		case ManTempPg:
			while(!cancelled ){
				Serial.print(F("Man TempPg here. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
				Serial.print(F("Ram free"));Serial.println(ramApp.freeRam());//delay(10);
				pause();
			}
			cancelled = false;
			return;
			break;
		case ProfSelPg:
			while(!cancelled ){
				Serial.print(F("Profile Selection here. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
				pause();
			}
			cancelled = false;
			return;
			break;
		case StrtStpPg:
			while(!cancelled ){
				Serial.print(F("Start Stop here. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
				pause();
			}
			cancelled = false;
			return;
			break;
		case RestartPg:
			while(!cancelled ){
				Serial.print(F("Restart here. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
				pause();
			}
			cancelled = false;
			return;
			break;
		default:
			break;
	}
}

void ProfileController::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR

	Serial.print(F("Active page . Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
	if(ProfileController::activePage == ProfileControlId){ // If we're on the main page, then give the rotary action to the local menu
		ProfileController::localMenu->rotaryAction(type, level, direction, param);
	}else{

			Serial.print(F("PC Rotary P:"));Serial.println(param);
		switch(type){
			case RotaryAction::ROTATE:
//						Serial.print(F("Temperature Rotate"));
				switch(direction){
					case RSE::FW:
//						Serial.println(F("Temperature Rotate FW"));
						if(ProfileController::targetTemp<200)
							ProfileController::targetTemp++;
						break;
					case RSE::RV:
//						Serial.println(F("Temperature Rotate RV"));
						if(ProfileController::targetTemp > 0)
							ProfileController::targetTemp--;
						break;
					case RSE::NC:
//						Serial.println(F("Temperature Rotate NC"));
						break;
				}
				break;
			case RotaryAction::SELECT:
				if(level == ButtonAction::BUTTONLOW){
//				ProfileController::tempControlAuto = !ProfileController::tempControlAuto;
				}else{
//						Serial.println(F("Temperature ButtonAction HIGH"));
				}
			break;
		}
	}
}
