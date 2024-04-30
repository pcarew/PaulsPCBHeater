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

bool cancelled = false;
volatile unsigned long time = 0;
unsigned long nextDisplayTime = 0;


//DisplayText *welcome;
//DisplayText *flash;

class CancelButton : Implements ButtonAction{
private:
public:
	ButtonAction::Level btLevel = ButtonAction::Level::BUTTONLOW;
	int btParam = 0;
	CancelButton(){}
	void createButton(int id){
		if(id>= 0 && id < 8){
//			Serial.println(F("Creating PortD Button"));
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
//		Serial.print(F("STID:"));Serial.println(currt->tid);
	PortDButton::buttonCheck(PIND,millis());
//		Serial.print(F("FTID:"));Serial.println(currt->tid);
}


char dispBuff[27];
Display myDisp = Display(0,0,255,255,255,255);
DisplayText displayElement = DisplayText(NULL,&myDisp, 1 , 1, 2 );

Ram ramApp;
TemperatureMonitoring temps;
HeaterController heater;
//Menu *menu = NULL;
MenuItem myMenu[] = {
		{"Temps",&temps,1,false},
		{"Unused SRam",&ramApp,2,false},
		{"Heater",&heater,3,false}
};
#define	NUMBERITEMS ( sizeof(myMenu)/sizeof(MenuItem))
Menu menu = Menu( (MenuItem*)myMenu, NUMBERITEMS, 5,6,4,&myDisp,2,1); // PinA:5, PinB:6, PinSel:4, starting at Row 2, col 1

//SDFileManagement *sdManager;

void setup()
{

	Serial.begin(115200);
	Serial.println(F("Setup Started"));

//	myDisp.setup();

	cnclButton = new CancelButton();
	cnclButton->createButton(3);

//	int numberItems = sizeof(myMenu)/sizeof(MenuItem);
//	menu = new Menu((MenuItem*)myMenu, numberItems, 5,6,4,myDisp,2,1); // PinA:5, PinB:6, PinSel:4, starting at Row 2, col 1

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

	menu.menuInvoke();

	displayElement.setText((char *)dispBuff);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);
	displayElement.setCol(1);
	displayElement.setRow(0);
	sprintf(dispBuff, "H:%s A:%d",
			HeaterController::heaterEnabled?"On  ":"Off ",
			(int) TemperatureMonitoring::ambient.getTemperature()
			);
	displayElement.show();

	displayElement.setRow(1);
	sprintf(dispBuff, "B:%d   T:%d",
			(int) TemperatureMonitoring::brdBot.getTemperature(),
			(int) TemperatureMonitoring::brdTop.getTemperature()
			);
	displayElement.show();

	/*
	if(cancelled){
	  	Serial.print(F("Button Level:"));
	  	Serial.print(cnclButton->btLevel,HEX);
	  	Serial.print(F(" Param:"));
	  	Serial.println(cnclButton->btParam);
	  	cancelled = false;
	}
	*/
//	chack_all_tcb();
//			endTime = millis();
//			threadAvgTime = (threadAvgTime * (TSKAVGCNT-1) + (endTime - startTime)) / TSKAVGCNT;
//			threadAvgTime = (endTime - startTime);
	pause();
}

#endif
