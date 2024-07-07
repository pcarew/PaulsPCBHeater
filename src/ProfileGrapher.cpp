/*
 * Graph.cpp
 *
 *  Created on: Jul 2, 2024
 *      Author: PaulCarew
 */

#include "pos/pos.h"
#include "ProfileGrapher.h"
#include "SystemData.h"

unsigned char ProfileGrapher::results[2][NUMRESULTS] = {0};		// 1st row is Guard results, 2nd is target results
unsigned char ProfileGrapher::currentBucket = 0;				// The bucket being accumulated

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define WHITE ST7735_WHITE
#define BLUE ST7735_BLUE

#define YPOS(y)	(height-y)
#define XPOS(x) (x)
#define XPOSCHR(xchr)	(XPOS((xchr)))
#define YPOSCHR(ychr)	(YPOS((ychr+10)))

#define ORIGINX   (XPOS(10))
#define ORIGINY  (YPOS(10))

void ProfileGrapher::menuAction(int param){
	int width = systemDisplay.tftScreen.width();
	int height = systemDisplay.tftScreen.height();
// position of the line on screen
	int xPos = 0;
	int yPos = 0;
	int oldXPos = 0;
	int oldYPos =-1; //height/2;

	systemDisplay.clear(255,0,255);

//	systemDisplay.tftScreen.drawLine(10,height-50,width,height-50,WHITE);					// X
//	systemDisplay.tftScreen.drawLine(10,height-50,10,0,WHITE);								// Y

	systemDisplay.tftScreen.drawLine(XPOS(10),YPOS(10),XPOS(width),YPOS(10),WHITE);		// XAccess
	systemDisplay.tftScreen.drawLine(XPOS(10),YPOS(10),XPOS(10),YPOS(height),WHITE);	// YAccess
	systemDisplay.tftScreen.setCursor(XPOSCHR(0), YPOSCHR(0));
	systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
	systemDisplay.tftScreen.setTextSize(1);
//   	systemDisplay.tftScreen.print(128);
   	systemDisplay.tftScreen.print(F("0,0"));

	Serial.print(F("H:"));Serial.print(height);
	Serial.print(F(" W:"));Serial.println(width);
	while(!cancelled){
		/*
		xDeg = (map(xPos,0,159,0,360))+phase;
		xRad = xDeg*3.141592/180;
		sinValue = sin(xRad);
		colour = RGB(0,0,255);

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
				systemDisplay.clear(255,0,255);
				systemDisplay.tftScreen.drawLine(0,63,159,63,WHITE);
				systemDisplay.tftScreen.drawLine(80,0,80,127,WHITE);
				//    	TFTscreen.fillScreen(colour); // @suppress("Invalid arguments")
//				systemDisplay.tftScreen.setCursor(10, 10);
//				systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
				//    	TFTscreen.print(colour,HEX);
				phase = 0;
			}
			oldXPos = 0;
			oldYPos = -1; //=height/2;
		} else {
    // increment the horizontal position:
			xPos++;
		}
		*/
		pause();
	}
}


void ProfileGrapher::tempDataPacket(unsigned long time,unsigned guard,unsigned guardValue, unsigned target,unsigned targetValue){
	unsigned char bucket = BUCKET(time);
	unsigned bucketCapacity = (unsigned int)(pow(4,bucket));

	if(bucket> ProfileGrapher::currentBucket)
		ProfileGrapher::currentBucket = bucket;
	results[PROFILEGUARD][bucket] = BucketAVG(bucketCapacity, results[PROFILEGUARD][bucket],guardValue);
	results[PROFILETARGET][bucket] = BucketAVG(bucketCapacity, results[PROFILETARGET][bucket],targetValue);
}
void ProfileGrapher::startNewProfile(){
	for(int i = 0; i<2;i++){
		for(int j=0;j<NUMRESULTS;j++){
			results[i][j] = 0;
		}
	}
	ProfileGrapher::currentBucket = 0;
}

/**
 * Place holder version
 *
void ProfileGrapher::menuAction(int param){
	int height = systemDisplay.tftScreen.width();			// Device values are for portrait mode. We're using landscape
	int width = systemDisplay.tftScreen.height();			// Device values are for portrait mode. We're using landscape
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
		colour = RGB(0,0,255);

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
				systemDisplay.clear(255,0,255);
				systemDisplay.tftScreen.drawLine(0,63,159,63,WHITE);
				systemDisplay.tftScreen.drawLine(80,0,80,127,WHITE);
				//    	TFTscreen.fillScreen(colour); // @suppress("Invalid arguments")
//				systemDisplay.tftScreen.setCursor(10, 10);
//				systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
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
*/
