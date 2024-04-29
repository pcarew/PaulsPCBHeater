#include "TempMonitoring.h"

#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "TempProbe.h"
#include "pos/pos.h"
#include "Ram.h"		// AJPC

IMPORT Display myDisp;
IMPORT DisplayText displayElement;
IMPORT char dispBuff[];

IMPORT bool cancelled;
IMPORT volatile unsigned long time;
IMPORT unsigned long nextDisplayTime;

IMPORT Ram  ramApp;

TempProbe TempMonitoring::ambient('A', AMBIANTSENSOR);
TempProbe TempMonitoring::brdBot('B', BRDBOT_SENSOR);
TempProbe TempMonitoring::brdTop('T', BRDTOP_SENSOR);

void TempMonitoring::update(){
	// Running on TaskA thread
	TempMonitoring::ambient.update();
	TempMonitoring::brdBot.update();
	TempMonitoring::brdTop.update();

}

void TempMonitoring::menuAction(int param) {
	// Running on System UI thread
	static const char *fmt = "%s: %d.%dC  ";

	displayElement.setText(dispBuff);

	myDisp.tftScreen.background(0,255,0);
	displayElement.setText(dispBuff);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);

	while(!cancelled){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time > nextDisplayTime){

			double tempReading = TempMonitoring::ambient.getTemperature();
			sprintf(dispBuff, fmt,"Amb",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setCol(0); displayElement.setRow(2); displayElement.setText(dispBuff); displayElement.show();
			Serial.println(dispBuff);

			tempReading = TempMonitoring::brdBot.getTemperature();
			sprintf(dispBuff, fmt,"Bot",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setCol(0); displayElement.setRow(3); displayElement.setText(dispBuff); displayElement.show();
			Serial.println(dispBuff);

			tempReading = TempMonitoring::brdTop.getTemperature();
			sprintf(dispBuff, fmt,"Top",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setCol(0); displayElement.setRow(4); displayElement.setText(dispBuff); displayElement.show();
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
