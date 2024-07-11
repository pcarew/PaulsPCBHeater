/*
 * Graph.cpp
 *
 *  Created on: Jul 2, 2024
 *      Author: PaulCarew
 */

#include "pos/pos.h"
#include "ProfileGrapher.h"
#include "SystemData.h"
#include "PaulsPCBHeater.h"

unsigned char ProfileGrapher::results[2][NUMRESULTS] = {0};		// 1st row is Guard results, 2nd is target results
unsigned char ProfileGrapher::currentBucket = 0;				// The bucket being accumulated
unsigned char ProfileGrapher::guard = 0;
unsigned char ProfileGrapher::target = 0;

const PROGMEM unsigned char YLabels[] = {
//		1,2, 5, 11,23, 52
		1,2, 4, 10,23, 52
};

#define width ( systemDisplay.tftScreen.width())
#define height ( systemDisplay.tftScreen.height())

#define RGB(r, g, b)	(((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))
#define WHITE			ST7735_WHITE
#define BLUE			ST7735_BLUE
#define DBLUE			RGB(0,0,139)
#define RED				ST7735_RED
#define DROSE			RGB(128,0,64)
#define GREEN			ST7735_GREEN
#define CYAN			ST7735_CYAN
#define LBLUE			RGB(143,217,251)
#define PINK			RGB(255,193,203)
#define BLACK			ST7735_BLACK

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
#define xAxisBktMax		(sizeof(YLabels)/sizeof(unsigned char))
#define xAxisLabelInc	1
#define xAxisScale		((double)xAxisLength/((double)NUMRESULTS+1.0))


double BUCKET(unsigned  seconds){
//	Serial.print(F("S:"));Serial.print(seconds);
	if( seconds<60)
		return 0;
	else if(seconds > 3071)
		return 15.0;
	else{
		double  value = log( (double)seconds/60) / LG1_3 +0.9;

//		Serial.print(F(" B:"));Serial.print(value);
//		Serial.print("<-");
		return  value;
	}
}

void ProfileGrapher::menuAction(int param){
	unsigned long time;
	int xPos = 0;
	int yPos = 0;
	int oldGXPos = ORIGINX;
	int oldGYPos = ORIGINY;
	int oldTXPos = ORIGINX;
	int oldTYPos = ORIGINY;


	// Target Temp and upper guard
//   	int ytarget	=  YPOS ((int) ( (ProfileGrapher::target*yAxisScale) + (double)ORIGINY));
   	int yguard	=  ( (ProfileGrapher::guard *yAxisScale) + (double)ORIGINY);
   	int ytarget	=  ( (ProfileGrapher::target*yAxisScale) + (double)ORIGINY);

	systemDisplay.clear(0,0,0);
	ProfileGrapher::drawAxis();
	systemDisplay.tftScreen.drawLine(XPOS((ORIGINX+1)),YPOS(yguard),XPOS(width),YPOS(yguard),RED);		// Guard Temp
	systemDisplay.tftScreen.drawLine(XPOS((ORIGINX+1)),YPOS(ytarget),XPOS(width),YPOS(ytarget),LBLUE);	// Target Temp

	nextDisplayTime = 0;
	while(!cancelled){
		time = millis();
		if(time>nextDisplayTime){
			nextDisplayTime = time+GRAPHUPDATETIME;
			oldGXPos = ORIGINX;
			oldGYPos = ORIGINY;
			oldTXPos = ORIGINX;
			oldTYPos = ORIGINY;
			for(int bucket=0;bucket <= ProfileGrapher::currentBucket;bucket++){
				xPos = ((bucket+1)*xAxisScale)+ORIGINX;

				yPos = ProfileGrapher::results[PROFILEGUARD][bucket] * yAxisScale+ORIGINY;
#ifdef PCBTEST
				yPos += 15;
#endif
				systemDisplay.tftScreen.drawLine(XPOS(oldGXPos),YPOS(oldGYPos),XPOS(xPos),YPOS(yPos),PINK);		// Guard Temp, PINK
				oldGXPos = xPos;
				oldGYPos = yPos;

//				Serial.println(ProfileGrapher::results[PROFILETARGET][bucket]);
				yPos = (ProfileGrapher::results[PROFILETARGET][bucket]) * yAxisScale+ORIGINY;
				systemDisplay.tftScreen.drawLine(XPOS(oldTXPos),YPOS(oldTYPos),XPOS(xPos),YPOS(yPos),BLUE);		// Target Temp
				oldTXPos = xPos;
				oldTYPos = yPos;
			}
		}
		pause();
	}
}

void ProfileGrapher::drawAxis(){
	char buff[12];

	systemDisplay.tftScreen.drawLine(XPOS(ORIGINX),YPOS(ORIGINY),XPOS((width-ORIGINX)),YPOS(ORIGINY),WHITE);	// XAccess
	systemDisplay.tftScreen.drawLine(XPOS(ORIGINX),YPOS(ORIGINY),XPOS(ORIGINX),YPOS((height-ORIGINY)),WHITE);	// YAccess

	systemDisplay.tftScreen.setCursor(XPOSCHR(0), YPOSCHR(0));
	systemDisplay.tftScreen.setTextColor(WHITE, BLACK);
	systemDisplay.tftScreen.setTextSize(1);
   	systemDisplay.tftScreen.print(0);

//   	Serial.println(xAxisScale);
   	// Ylabels
   	for(double yaxisDeg = yAxisLabelInc;yaxisDeg <yAxisDegMax; yaxisDeg += yAxisLabelInc){
   		int ypos =  YPOSCHR((int)(yaxisDeg*yAxisScale));
   		systemDisplay.tftScreen.setCursor(XPOSCHR(0), ypos);
   		systemDisplay.tftScreen.print((int)yaxisDeg);
   	}

   	// XLabels
   	for(int xaxisLblId = 0;xaxisLblId <xAxisBktMax; xaxisLblId += xAxisLabelInc){
   		unsigned label = (unsigned)pgm_read_byte(&YLabels[xaxisLblId]);
   		double bkt = BUCKET((unsigned)label*60);	// Turn it into seconds
   		int xpos =  (int)(((bkt+1.0)*xAxisScale)+ORIGINX);
   		if(label >= 10) xpos -= 10;							// for double digit lables, off set left by a digit
//   	Serial.print(xaxisLblId);
//   	Serial.print(",");
//   	Serial.print(label);
//   	Serial.print(",");
//   	Serial.print(bkt);
//   	Serial.print(",");
//   	Serial.println(xpos);delay(100);
   		/*
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
   		*/
//   		sprintf_P(buff,PSTR("%dm"),minutes);

   		systemDisplay.tftScreen.setCursor(XPOSCHR(xpos), YPOSCHR(0));
   		systemDisplay.tftScreen.print(label);
   	}
   	sprintf_P(buff,PSTR("T:%d G:%d"),ProfileGrapher::target,ProfileGrapher::guard);
	systemDisplay.tftScreen.setTextColor(WHITE, BLUE);
	systemDisplay.tftScreen.setCursor(XPOSCHR(90), YPOSCHR(118));
	systemDisplay.tftScreen.print(buff);
}

void ProfileGrapher::tempDataPacket(unsigned long time,unsigned guardValue, unsigned targetValue){
	unsigned char bucket = (unsigned char) BUCKET(time);

	if(bucket> ProfileGrapher::currentBucket && bucket < NUMRESULTS){
		results[PROFILEGUARD][bucket] = results[PROFILEGUARD][ProfileGrapher::currentBucket];			// Copy over previous bucket to new bucket
		results[PROFILETARGET][bucket] = results[PROFILETARGET][ProfileGrapher::currentBucket];			// Copy over previous bucket to new bucket
		ProfileGrapher::currentBucket = bucket;
	}
	results[PROFILEGUARD][bucket] = BucketAVG(results[PROFILEGUARD][bucket],guardValue);		// Update with new value
	results[PROFILETARGET][bucket] = BucketAVG(results[PROFILETARGET][bucket],targetValue);		// Update with new value
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

