#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "TempProbe.h"
#include "TempDisplay.h"
#include "pos/pos.h"
#include "Ram.h"		// AJPC

extern Display *myDisp;
extern DisplayText *displayElement;
extern char dispBuff[];

TempProbe TempDisplay::ambient('A', AMBIANTSENSOR);
TempProbe TempDisplay::brdBot('B', BRDBOT_SENSOR);
TempProbe TempDisplay::brdTop('T', BRDTOP_SENSOR);

void TempDisplay::update(){
	// Running on TaskA thread
	TempDisplay::ambient.update();
	TempDisplay::brdBot.update();
	TempDisplay::brdTop.update();

}
extern Ram  ramApp;

void TempDisplay::menuAction(int param) {
	// Running on System UI thread
	static const char *fmt = "%s: %d.%dC  ";

	displayElement->setText(dispBuff);

	myDisp->tftScreen.background(0,255,0);
	displayElement->setText(dispBuff);
	displayElement->setBg(0, 255, 0);
	displayElement->setFg(255, 0, 0);

	while(!cancelled){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time>nextDisplayTime){

			double tempReading = TempDisplay::ambient.getTemperature();
			sprintf(dispBuff, fmt,"Amb",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement->setCol(0); displayElement->setRow(2); displayElement->setText(dispBuff); displayElement->show();
			Serial.println(dispBuff);

			tempReading = TempDisplay::brdBot.getTemperature();
			sprintf(dispBuff, fmt,"Bot",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement->setCol(0); displayElement->setRow(3); displayElement->setText(dispBuff); displayElement->show();
			Serial.println(dispBuff);

			tempReading = TempDisplay::brdTop.getTemperature();
			sprintf(dispBuff, fmt,"Top",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement->setCol(0); displayElement->setRow(4); displayElement->setText(dispBuff); displayElement->show();
			Serial.println(dispBuff);

			nextDisplayTime = time+2050l;
			Serial.print(F("Ram free"));Serial.println(ramApp.freeRam());//delay(10);
		}
		pause();
	}
}

/*
void TempDisplay::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT
			Serial.print(F("TD Dir:"));Serial.println(direction);
}
*/
