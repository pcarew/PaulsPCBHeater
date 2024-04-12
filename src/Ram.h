/*
 * Ram.h
 *
 *  Created on: Apr 4, 2024
 *      Author: PaulCarew
 */

#ifndef RAM_H_
#define RAM_H_
#include "Menu.h"
#include "DIsplay.h"
#include "pos/pos.h"


extern Display *myDisp;
extern bool cancelled;
extern DisplayText *displayElement;

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
	void action(int param){
		char dispBuff[27];
		const char *fmt = "SRAM: %d";
		displayElement->setText(dispBuff);

		DisplayText dispElement(dispBuff, myDisp, 1, 1, 2);
		myDisp->tftScreen.background(0,0,0);
		sprintf(dispBuff, fmt,this->freeRam());

		dispElement.setBg(0, 0, 0);
		dispElement.setFg(255, 255, 255);
		dispElement.setCol(0);
		dispElement.setRow(3);
		dispElement.setText(dispBuff);
		dispElement.show();

		Serial.println(dispBuff);
		while(!cancelled){
			pause();
		}
	}
};

#endif /* RAM_H_ */
