//#include "SDFileManagement.h"
#include "pos/pos.h"
#include "Button.h"
#include "Menu.h"
#include "TempDisplay.h"
#include "HeaterControl.h"
#include "PCBHeaterTasks.h"
#include "Ram.h"
#include "Display.h"

bool cancelled = false;
volatile unsigned long time = 0;
unsigned long nextDisplayTime = 0;

Display *myDisp; //= new Display(0,0,255,255,255,255);
DisplayText *displayElement;//	= new DisplayText(NULL,myDisp, 1 , 1, 2 );
char dispBuff[27];

DisplayText *welcome;
DisplayText *flash;

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
	HeaterControl::checkACZeroCrossing();		// A0 -> A5
	PortCButton::buttonCheck(PINC,millis());
}

ISR (PCINT2_vect) {			// D0 -> D7 PortD
//		Serial.print(F("STID:"));Serial.println(currt->tid);
	PortDButton::buttonCheck(PIND,millis());
//		Serial.print(F("FTID:"));Serial.println(currt->tid);
}


Ram ramApp;
TempDisplay *temps = new TempDisplay();
HeaterControl *heater = new HeaterControl();
Menu *menu = NULL;
MenuItem myMenu[] = {
		{"Temps",temps,1,false},
		{"Unused SRam",&ramApp,2,false},
		{"Heater",heater,3,false}
};

//SDFileManagement *sdManager;

void setup()
{

	Serial.begin(115200);
	myDisp = new Display(0,0,255,255,255,255);
	displayElement	= new DisplayText(NULL,myDisp, 1 , 1, 2 );

	cnclButton = new CancelButton();
	cnclButton->createButton(3);

	int numberItems = sizeof(myMenu)/sizeof(MenuItem);
	menu = new Menu((MenuItem*)myMenu, numberItems, 5,6,4,myDisp); // PinA:5, PinB:6, PinSel:4

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

	menu->menuInvoke();
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
