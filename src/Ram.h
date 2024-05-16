/*
 * Ram.h
 *
 *  Created on: Apr 4, 2024
 *      Author: PaulCarew
 */

#ifndef RAM_H_
#define RAM_H_
#include "Menu.h"
#include "Display.h"
#include "pos/pos.h"


IMPORT Display systemDisplay;
IMPORT bool cancelled;
IMPORT DisplayText displayElement;

class Ram: Implements MenuAction{
private:
public:
	Ram(){}
	~Ram(){}

	int freeRam() {
  		extern int __heap_start,*__brkval;
  		int stackPositionPtr;
  		return (int)&stackPositionPtr - (__brkval == 0 ?
		  	  	(int)&__heap_start : (int) __brkval);
	}
	/*
	void ramDump(unsigned start, unsigned size){
		for(unsigned i = 0; i < size ; i++){
			if(!(i%16)){ Serial.println();Serial.print((unsigned)start+i,HEX);}
			if(!(i%8)){ Serial.print(F(" : ")); }
			Serial.print(F(" "));Serial.print(*(unsigned char *)(start+i),HEX);

		}
		Serial.println();
	}
	*/
	/*
	static void intrState(){
		Serial.print(F("Intr:")); Serial.println((SREG & (1<<7))?F("Enabled"):F("Disabled"));
	}
	*/
	void menuAction(int param){
		char dispBuff[27];
		const char *fmt = "SRAM: %d";
		displayElement.setText(dispBuff);

		systemDisplay.clear(0,0,0);
		sprintf(dispBuff, fmt,this->freeRam());

		displayElement.setBg(0, 0, 0);
		displayElement.setFg(255, 255, 255);
		displayElement.setCol(0);
		displayElement.setRow(3);
		displayElement.setText(dispBuff);
		displayElement.show();

		Serial.println(dispBuff);
		while(!cancelled){
			pause();
		}
	}
};

#endif /* RAM_H_ */
