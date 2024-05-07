/*
 * ProfileController.cpp
 *
 *  Created on: May 2, 2024
 *      Author: PaulCarew
 */

#include "ProfileController.h"
#include "Menu.h"
#include "pos/pos.h"

ProfileController::ProfileController() {
	// TODO Auto-generated constructor stub

}

ProfileController::~ProfileController() {
	// TODO Auto-generated destructor stub
}



// Used for UI
extern bool cancelled;
extern Display systemDisplay;
extern DisplayText displayElement;
extern volatile unsigned long time;
extern unsigned long nextDisplayTime;
	// Menu buffers
extern char dispBuff[];
const char *ProfileController::fmt = "Profile Control";

extern ProfileController profileController;

unsigned char ProfileController::targetTemp = 44;

#define	NUMBERITEMS 4
MenuItem *ProfileController::localMenuItems = new MenuItem[NUMBERITEMS] {
		{"ManTemp",&profileController,40,false},
		{"ProfileSel",&profileController,41,false},
		{"Start/Stop",&profileController,42,false},
		{"Restart",&profileController,43,false}
};

/*
MenuItem profileControllerMenuItems[] {
		{"ManTemp",&profileController,40,false},
		{"ProfileSel",&profileController,41,false},
		{"Start/Stop",&profileController,42,false},
		{"Restart",&profileController,43,false}
};
*/

//#define	NUMBERITEMS ( sizeof(ProfileController::localMenuItems)/sizeof(MenuItem))

Menu* ProfileController::localMenu = new Menu( (MenuItem*)ProfileController::localMenuItems, NUMBERITEMS, (RotarySelector*)NULL,&systemDisplay,2,1); //  starting at Row 2, col 1
char ProfileController::activePage = 0;

// User Interface for Profile Controller. Runs under System UI Thread
void ProfileController::menuAction(int param){

	ProfileController::activePage = param;
	systemDisplay.tftScreen.background(0,255,0);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	ProfileController::localMenu->currentMenuItemId = -1;
	while(!cancelled ){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(ProfileController::activePage < 40){

//			Serial.println(F("Invoking local menu"));
			ProfileController::localMenu->menuInvoke();						// **** This is a recursive call ***
			if(time>nextDisplayTime){
				nextDisplayTime = time+500l;
				displayElement.setCol(0);  displayElement.setText((char *)dispBuff);

				sprintf(dispBuff, "ProfContr");
			 	displayElement.setRow(0);  displayElement.show();
				sprintf(dispBuff, "Tmp: %d  ", ProfileController::targetTemp);
			 	displayElement.setRow(1);  displayElement.show();
			}
		}else{
			while(!cancelled ){
				Serial.print(F("local menu here. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
				pause();
			}
			Serial.print(F("local page ended. Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
			ProfileController::activePage = 0;
//			ProfileController::localMenu->inMenu = true;
			cancelled = false;
			return;														// *** Terminate the recursive call
		}
		pause();
	}
	ProfileController::activePage = 0;
}

void ProfileController::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT Called from PortD ISR

	Serial.print(F("Active page . Pg:"));Serial.println((int)ProfileController::activePage);delay(500);
	if(ProfileController::activePage<40){ // If we're on the main page, then give the rotary action to the local menu
		ProfileController::localMenu->rotaryAction(type, level, direction, param);
	}else{

//			Serial.print(F("HC Dir:"));Serial.println(direction);
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
