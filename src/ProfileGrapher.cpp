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
unsigned char ProfileGrapher::guard = 150;
unsigned char ProfileGrapher::target = 100;

#define width ( systemDisplay.tftScreen.width())
#define height ( systemDisplay.tftScreen.height())

#define RGB(r, g, b)	(((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define WHITE			ST7735_WHITE
#define BLUE			ST7735_BLUE
#define RED				ST7735_RED
#define GREEN			ST7735_GREEN

#define YPOS(y)			(height-y)
#define XPOS(x) 		(x)
#define XPOSCHR(xchr)	(XPOS((xchr)))
#define YPOSCHR(ychr)	(YPOS((ychr+10)))

#define ORIGINX  		(10)
#define ORIGINY  		(10)

#define yAxisLength 	 (height-ORIGINY)
#define yAxisDegMax		((ProfileGrapher::guard*1.25))
#define yAxisLabelInc	((yAxisDegMax/5.0))
#define yAxisScale		((double)yAxisLength/(double)yAxisDegMax)

#define xAxisLength 	(width-ORIGINX)
#define xAxisBktMax		NUMRESULTS
#define xAxisLabelInc	1
#define xAxisScale		((double)xAxisLength/(double)xAxisBktMax)


void ProfileGrapher::menuAction(int param){

	systemDisplay.clear(255,0,255);
	ProfileGrapher::drawAxis();

	// Target Temp and upper guard
//   	int ytarget	=  YPOS ((int) ( (ProfileGrapher::target*yAxisScale) + (double)ORIGINY));
   	int ytarget	=  ( (ProfileGrapher::target*yAxisScale) + (double)ORIGINY);
   	int yguard	=  ( (ProfileGrapher::guard *yAxisScale) + (double)ORIGINY);
//   			Serial.print(ProfileGrapher::target);
//   			Serial.print(",");
//   			Serial.print(yAxisScale);
//   			Serial.print(",");
//   			Serial.print(ytarget);
//   			Serial.print(",");
//   			Serial.print(YPOS(ytarget));
//   			Serial.print(":");
//   			Serial.print(ProfileGrapher::guard);
//   			Serial.print(",");
//   			Serial.print(yAxisScale);
//   			Serial.print(",");
//   			Serial.print(yguard);
//   			Serial.print(",");
//   			Serial.println(YPOS(yguard));
	systemDisplay.tftScreen.drawLine(XPOS((ORIGINX+1)),YPOS(ytarget),XPOS(width),YPOS(ytarget),BLUE);	// Target Temp
	systemDisplay.tftScreen.drawLine(XPOS((ORIGINX+1)),YPOS(yguard),XPOS(width),YPOS(yguard),RED);		// Guard Temp

	while(!cancelled){
		pause();
	}
}

void ProfileGrapher::drawAxis(){
	char buff[12];

	systemDisplay.tftScreen.drawLine(XPOS(ORIGINX),YPOS(ORIGINY),XPOS((width-ORIGINX)),YPOS(ORIGINY),WHITE);	// XAccess
	systemDisplay.tftScreen.drawLine(XPOS(ORIGINX),YPOS(ORIGINY),XPOS(ORIGINX),YPOS((height-ORIGINY)),WHITE);	// YAccess

	systemDisplay.tftScreen.setCursor(XPOSCHR(0), YPOSCHR(0));
	systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
	systemDisplay.tftScreen.setTextSize(1);
   	systemDisplay.tftScreen.print(0);

   	// Ylabels
   	for(double yaxisDeg = yAxisLabelInc;yaxisDeg <yAxisDegMax; yaxisDeg += yAxisLabelInc){
   		int ypos =  YPOSCHR((int)(yaxisDeg*yAxisScale));
//   			Serial.print(yaxisDeg);
//   			Serial.print(",");
//   			Serial.print(yAxisScale);
//   			Serial.print(",");
//   			Serial.println(ypos);
   		systemDisplay.tftScreen.setCursor(XPOSCHR(0), ypos);
   		systemDisplay.tftScreen.print((int)yaxisDeg);
   	}

   	// XLabels
   	for(double xaxisBkt = xAxisLabelInc;xaxisBkt <xAxisBktMax; xaxisBkt += xAxisLabelInc){
   		int xpos =  XPOSCHR((int)(xaxisBkt*xAxisScale));
   		double t = pow(4,xaxisBkt)+0.01;
//	   			Serial.print(xaxisBkt);
//   			Serial.print(",");
//   			Serial.print(xAxisScale);
//   			Serial.print(",");
//   			Serial.println(xpos);
//   			Serial.print(t);
//				Serial.print(",");
//				Serial.println((int)t);

   		int value;
   		char sym;
   		if(xaxisBkt<4){
   				value = (int)t;
   				sym = 's';
   		}else if(xaxisBkt<6){
   			value = (int) (t/60.0);
   			sym = 'm';
   		}else{
   			value = (int)(t/3600.0);
   			sym = 'h';
   		}
   		sprintf_P(buff,PSTR("%d%c"),value,sym);

//   		systemDisplay.tftScreen.setCursor(XPOS(xpos), YPOSCHR(ORIGINY));
//   		systemDisplay.tftScreen.print("|");
   		systemDisplay.tftScreen.setCursor(XPOS(xpos), YPOSCHR(0));
   		systemDisplay.tftScreen.print(buff);
   	}
   	sprintf_P(buff,PSTR("T:%d G:%d"),ProfileGrapher::target,ProfileGrapher::guard);
	systemDisplay.tftScreen.setCursor(XPOSCHR(90), YPOSCHR(118));
	systemDisplay.tftScreen.print(buff);
}

void ProfileGrapher::tempDataPacket(unsigned long time,unsigned guardValue, unsigned targetValue){
	unsigned char bucket = BUCKET(time);
	unsigned bucketCapacity = (unsigned int)(pow(4,bucket));

	if(bucket> ProfileGrapher::currentBucket)
		ProfileGrapher::currentBucket = bucket;
	results[PROFILEGUARD][bucket] = BucketAVG(bucketCapacity, results[PROFILEGUARD][bucket],guardValue);
	results[PROFILETARGET][bucket] = BucketAVG(bucketCapacity, results[PROFILETARGET][bucket],targetValue);
}

void ProfileGrapher::startNewProfile(unsigned guard, unsigned target){
	ProfileGrapher::guard = guard;
	ProfileGrapher::target = target;

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
