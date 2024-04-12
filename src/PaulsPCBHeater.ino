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
unsigned long nextSampleTime = 0;

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
Menu *menu = NULL;
MenuItem myMenu[] = {
		{"Temps",temps,1,false},
		{"Unused SRam",&ramApp,2,false},
		{"Heater",new HeaterControl(),3,false}
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

//	sdManager = new SDFileManagement();
//	sdManager->testSD();

//	sdManager->~SDFileManagement();

//	welcome = new DisplayText(20 ,myDisp, 0, 0, 2);
//	welcome->show((char *)F("Welcome"));

//	flash = new DisplayText(20 ,myDisp, 8, 0, 2);
//	flash->setText("Time");


//	Serial.print(F("startup Handler: "));Serial.println((unsigned int)(myMenu->handler),HEX);
//	Serial.print(F("startup Handler act: "));Serial.println((unsigned int)(myMenu->handler->action),HEX);
//	Serial.print(F("startup temps: "));Serial.println((unsigned int)(temps),HEX);

	PCBHeaterTasks::startup();

	Serial.println(F("Setup Done"));
}

void loop()
{
  	Serial.println(F("Loop"));
	time = millis();

	menu->update();
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
	pause();
}
