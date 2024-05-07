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

#define SystemId			0
#define TempMonId			10
#define HeaterConrolId		20
#define ProfileControlId	30

IMPORT bool cancelled;
IMPORT volatile unsigned long time;
IMPORT unsigned long nextDisplayTime;

IMPORT char dispBuff[27];
IMPORT Display systemDisplay;
IMPORT DisplayText displayElement;

/* System Components */
IMPORT TemperatureMonitoring tempMonitor;
IMPORT HeaterController heaterController;
IMPORT ProfileController profileController;

#endif /* SYSTEMDATA_H_ */
