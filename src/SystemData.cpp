/*
 * SystemData.cpp
 *
 *  Created on: May 7, 2024
 *      Author: PaulCarew
 */

#include "SystemData.h"

bool cancelled = false;
volatile unsigned long time = 0;
unsigned long nextDisplayTime = 0;

char dispBuff[DISPBUFFLEN];
Display systemDisplay = Display(0,0,255,255,255,255);
DisplayText displayElement = DisplayText(NULL,&systemDisplay, 1 , 1, 2 );

/* System Components */
TemperatureMonitoring tempMonitor;
HeaterController heaterController;
ProfileController profileController;
Graph profileGrapher;

Ram ramApp;
