#include "TemperatureMonitoring.h"

#include <arduino.h>
#include "Button.h"
#include "Display.h"
#include "pos/pos.h"
#include "Ram.h"		// AJPC
#include "TemperatureProbe.h"

IMPORT Display systemDisplay;
IMPORT DisplayText displayElement;
IMPORT char dispBuff[];

IMPORT bool cancelled;
IMPORT volatile unsigned long time;
IMPORT unsigned long nextDisplayTime;

IMPORT Ram  ramApp;

TemperatureProbe TemperatureMonitoring::ambient('A', AMBIANTSENSOR);
TemperatureProbe TemperatureMonitoring::brdBot('B', BRDBOT_SENSOR);
TemperatureProbe TemperatureMonitoring::brdTop('T', BRDTOP_SENSOR);

void TemperatureMonitoring::update(){
	// Running on TaskA thread
	TemperatureMonitoring::ambient.update();
	TemperatureMonitoring::brdBot.update();
	TemperatureMonitoring::brdTop.update();

}

void TemperatureMonitoring::menuAction(int param) {
	// Running on System UI thread
	static const char *fmt = "%s: %d.%dC  ";
	double tempReading ;

	displayElement.setText(dispBuff);

	systemDisplay.clear(0,255,0);
	displayElement.setText(dispBuff);
	displayElement.setBg(0, 255, 0);
	displayElement.setFg(255, 0, 0);

	while(!cancelled){
		time = millis();					// As we've taken over control of the processor, we need to update time for everyon (and ourselves)
		if(time > nextDisplayTime){
			nextDisplayTime = time+2050l;

			displayElement.setText(dispBuff);
			displayElement.setCol(0);

			tempReading = TemperatureMonitoring::ambient.getTemperature();
			sprintf(dispBuff, fmt,"Amb",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setRow(2);  displayElement.show();
//			Serial.println(dispBuff);

			tempReading = TemperatureMonitoring::brdBot.getTemperature();
			sprintf(dispBuff, fmt,"Bot",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setRow(3);  displayElement.show();
//			Serial.println(dispBuff);

			tempReading = TemperatureMonitoring::brdTop.getTemperature();
			sprintf(dispBuff, fmt,"Top",(int)tempReading,(unsigned int)((unsigned long)(tempReading*100.0))%100);
			displayElement.setRow(4);  displayElement.show();
//			Serial.println(dispBuff);

//			Serial.print(F("Ram free"));Serial.println(ramApp.freeRam());//delay(10);
		}
		pause();
	}
}

/*
void TempDisplay::rotaryAction(const int type, int level, RSE::Dir direction, int param){		// type is ROTATE or SELECT
			Serial.print(F("TD Dir:"));Serial.println(direction);
}
*/
