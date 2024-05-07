#ifndef PaulsPCBHeater_INO
#define PaulsPCBHeater_INO
//#include "SDFileManagement.h"
#include "pos/pos.h"
#include "Button.h"
#include "Menu.h"
#include "PCBHeaterTasks.h"
#include "Ram.h"
#include "Display.h"
#include "HeaterController.h"
#include "TemperatureMonitoring.h"
#include "ProfileController.h"
#include "SystemData.h"


class CancelButton : Implements ButtonAction{
private:
public:
	ButtonAction::Level btLevel = ButtonAction::Level::BUTTONLOW;
	int btParam = 0;
	CancelButton(){}
	void createButton(int id){
		if(id>= 0 && id < 8){
			new PortDButton(id , this, id, 5);
			/*
		}else if(id >7 && id < 14){
			Serial.println(F("Creating PortB Button"));
			new PortBButton(id , this, id);
		}else if(id >= A0 && id <= A5){
			Serial.println(F("Creating PortC Button"));
			new PortCButton(id , this, id);
			*/
		}
	}
	void buttonAction(ButtonAction::Level level, int param){
		cancelled = true;
		this->btLevel = level;
		this->btParam = param;
	}
};

CancelButton *cnclButton;

ISR (PCINT0_vect) {			// D8 -> D13 PortB
	PortBButton::buttonCheck(PINB,millis());
}

ISR (PCINT1_vect) {
	HeaterController::checkACZeroCrossing();		// A0 -> A5
	PortCButton::buttonCheck(PINC,millis());
}

ISR (PCINT2_vect) {			// D0 -> D7 PortD
	PortDButton::buttonCheck(PIND,millis());
}



//Menu *menu = NULL;
MenuItem mainMenuItems[] = {
		{"Unused SRam",&ramApp,SystemId+1,false},
		{"Temps",&tempMonitor,TempMonId,false},
		{"Heater",&heaterController,HeaterConrolId,false},
		{"ProCont",&profileController,ProfileControlId,false}
};
#define	NUMBERITEMS ( sizeof(mainMenuItems)/sizeof(MenuItem))
Menu mainMenu = Menu( (MenuItem*)mainMenuItems, NUMBERITEMS, 5,6,4,&systemDisplay,2,1); // PinA:5, PinB:6, PinSel:4, starting at Row 2, col 1

void setup()
{

	Serial.begin(115200);
	Serial.println(F("Setup Started"));

	systemDisplay.setup();

	cnclButton = new CancelButton();
	cnclButton->createButton(3);

	HeaterController::setup();

	PCBHeaterTasks::startup();

	Serial.println(F("Setup Done"));
}

//IMPORT int pos_stats();
//IMPORT long taskAvgTime;
//IMPORT long taskASwitches;
//IMPORT long switches;
//#define TSKAVGCNT 2						// Averaged over 10 cycles
//long threadAvgTime = 1;
long threadSwitches = 0;
void loop()
{
//	static  long startTime;
//	static  long endTime;
//	static unsigned long periodEnd1 =0;
//		threadSwitches++;
//			startTime = millis();
//  	Serial.println(F("Loop"));
	time = millis();
	/*
	if(time>periodEnd1){
		periodEnd1 = time+1000;	// Every 1 second
		Serial.print(F(" POS Switches Hz:"));Serial.print(pos_stats()); Serial.print(F(" TaskA Avg: "));Serial.print(taskAvgTime);Serial.print(F(" TaskA Switches:"));Serial.println(taskASwitches);
		Serial.print(F(" POS Switches Hz:"));Serial.print(pos_stats()); Serial.print(F(" Loop  lst: "));Serial.print(threadAvgTime);Serial.print(F(" Loop  Switches:"));Serial.println(threadSwitches);
		taskASwitches = 0;
		threadSwitches = 0;
	}
	*/

	mainMenu.menuInvoke();

	displayElement.setText((char *)dispBuff);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	displayElement.setCol(1);
	displayElement.setRow(0);
	sprintf(dispBuff, "H:%s A:%3d",
			HeaterController::heaterEnabled?" On":"Off",
			(int) TemperatureMonitoring::ambient.getTemperature()
			);
	displayElement.show();

	displayElement.setRow(1);
	sprintf(dispBuff, "B:%3d T:%3d",
			(int) TemperatureMonitoring::brdBot.getTemperature(),
			(int) TemperatureMonitoring::brdTop.getTemperature()
			);
	displayElement.show();

//	chack_all_tcb();
//			endTime = millis();
//			threadAvgTime = (threadAvgTime * (TSKAVGCNT-1) + (endTime - startTime)) / TSKAVGCNT;
//			threadAvgTime = (endTime - startTime);
	pause();
}

#endif
