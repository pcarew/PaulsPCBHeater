#ifndef PaulsPCBHeater_INO
#define PaulsPCBHeater_INO

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
	PortDButton *btn;
	int btParamEnd = 0;
//	CancelButton(){}
	void createButton(int pin, int id){
//		Serial.println(F("Creating cancel button"));delay(20);
		if(pin>= 0 && pin < 8){
			this->btn = new PortDButton(pin , this, id, 5);
		}
		this->btParam = id;
		this->btParamEnd = id;
	}
	void buttonAction(ButtonAction::Level level, int param){
		this->btLevel = level;
		this->btParam = param;
		if(level == BUTTONLOW){
			cancelled = true;
//			Serial.println(F("CL"));delay(20);
		}else{
//			Serial.println(F("CH"));delay(20);
		}
//			Serial.print(F("TID:"));Serial.print(currt->tid);Serial.print(F(" ItrRam free"));Serial.println(ramApp.freeRam());
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
	cnclButton->createButton(3,90);

	HeaterController::setup();

	PCBHeaterTasks::startup();

	Serial.println(F("Setup Done"));
}

void topLevelStatus();
void loop()
{
	static unsigned long periodEnd1 =0;
	static unsigned long periodEnd2 =0;
	time = millis();
	if(time>periodEnd1){
		periodEnd1 = time+250l;	// Every 1/2 second

		mainMenu.menuInvoke();
	}
	if(time>periodEnd2){
		periodEnd2 = time+500l;	// Every 1/2 second

		topLevelStatus();
	}
		pause();
}
void topLevelStatus(){
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
}

#endif
