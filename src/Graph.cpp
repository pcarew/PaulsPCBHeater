/*
 * Graph.cpp
 *
 *  Created on: Jul 2, 2024
 *      Author: PaulCarew
 */

#include "pos/pos.h"
#include "Graph.h"
#include "SystemData.h"

const int Graph::height = systemDisplay.tftScreen.width();			// Device values are for portrait mode. We're using landscape
const int Graph::width = systemDisplay.tftScreen.height();			// Device values are for portrait mode. We're using landscape

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define WHITE ST7735_WHITE
#define BLUE ST7735_BLUE

void Graph::menuAction(int param){
// position of the line on screen
	int xPos = 0;
	int yPos = 0;
	int oldXPos = 0;
	int oldYPos =-1; //height/2;
	int phase = 0;

	double xDeg;
	double xRad;
	double sinValue;
	uint16_t colour;

	systemDisplay.clear(255,0,255);

	systemDisplay.tftScreen.drawLine(0,63,159,63,WHITE);
	systemDisplay.tftScreen.drawLine(80,0,80,127,WHITE);

	while(!cancelled){
		xDeg = (map(xPos,0,159,0,360))+phase;
		xRad = xDeg*3.141592/180;
		sinValue = sin(xRad);
		uint16_t colour = RGB(0,0,255);

		yPos = map(sinValue*100,-100,100.0,-(height-1)/2,(height-1)/2);
		yPos = height/2 - yPos-2;
		if(oldYPos == -1) oldYPos = yPos;

			// draw a line in a nice color
		systemDisplay.tftScreen.drawLine(oldXPos, oldYPos, xPos, yPos,WHITE);
		oldXPos = xPos;
		oldYPos = yPos;

			// if the graph has reached the screen edge
			// erase the screen and start again
		if (xPos >= 160) {
			xPos = 0;
		    phase += 40;
			if(phase>160){
				colour +=3;
				//    	TFTscreen.fillScreen(colour); // @suppress("Invalid arguments")
				systemDisplay.tftScreen.setCursor(10, 10);
				systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
				//    	TFTscreen.print(colour,HEX);
				phase = 0;
			}
			oldXPos = 0;
			oldYPos = -1; //=height/2;
		} else {
    // increment the horizontal position:
			xPos++;
		}
		pause();
	}
}

