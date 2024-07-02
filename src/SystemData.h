/*
 * SystemData.h
 *
 *  Created on: May 7, 2024
 *      Author: PaulCarew
 */

#ifndef SYSTEMDATA_H_
#define SYSTEMDATA_H_
#include "pos/pos.h"
#include "TemperatureMonitoring.h"
#include "TemperatureController.h"
#include "ProfileController.h"
#include "HeaterController.h"
#include "Display.h"
#include "Button.h"
#include "Ram.h"
#include "Graph.h"

#define SystemId			0
#define TempMonId			10
#define HeaterConrolId		20
#define ProfileControlId	30
#define ProfileGrapherId	40

#define DISPBUFFLEN			27

IMPORT Ram ramApp;

IMPORT bool cancelled;
IMPORT volatile unsigned long time;
IMPORT unsigned long nextDisplayTime;

IMPORT char dispBuff[DISPBUFFLEN];
IMPORT Display systemDisplay;
IMPORT DisplayText displayElement;

/* System Components */
IMPORT TemperatureMonitoring tempMonitor;
IMPORT HeaterController heaterController;
IMPORT ProfileController profileController;
IMPORT Graph profileGrapher;

#endif /* SYSTEMDATA_H_ */
