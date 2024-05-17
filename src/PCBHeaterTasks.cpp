/*
 * PCBHeaterTasks.cpp
 *
 *  Created on: Apr 3, 2024
 *      Author: PaulCarew
 */

#include "arduino.h"
#include "pos/pos.h"
#include "PCBHeaterTasks.h"

#include "HeaterController.h"
#include "TemperatureMonitoring.h"
#include "TemperatureController.h"

IMPORT TemperatureMonitoring tempMonitor;
IMPORT HeaterController *heaterController;

	//void (*TaskType)(volatile TCB *);
//#define TSKAVGCNT 2						// Averaged over 10 cycles
//long taskAvgTime = 1;
//long taskASwitches = 0;
void PCBHeaterTasks::taskA(volatile TCB *tcb){
//	volatile TCB *localTcb = tcb;
//	char taskLetter = (char)('A'+tcb->tid - 1);
//	char tid = tcb->tid;
//	static  long startTime;
//	static  long endTime;

	FOREVER{
//		taskASwitches++;
//		Serial.println(F("Task A"));
//			startTime = millis();
		tempMonitor.update();
//			endTime = millis();
		heaterController->process();

		TemperatureController::update();

//			taskAvgTime = (taskAvgTime * (TSKAVGCNT-1) + (endTime - startTime)) / TSKAVGCNT;
//			taskAvgTime = endTime - startTime;
		pause();
	}
}

void PCBHeaterTasks::taskB(volatile TCB *tcb){
//	volatile TCB *localTcb = tcb;
//	char taskLetter = (char)('A'+tcb->tid - 1);
//	char tid = tcb->tid;

	FOREVER{
//		Serial.println(F("Task B"));

		pause();
	}
}

void PCBHeaterTasks::taskC(volatile TCB *tcb){
//	volatile TCB *localTcb = tcb;
//	char taskLetter = (char)('A'+tcb->tid - 1);
//	char tid = tcb->tid;

	FOREVER{
//		Serial.println(F("Task C"));
		pause();
	}
}

#define PCBSTACKSIZE	(unsigned)110 //176 //240

/* Ensure that enough thread stack space (pos.h STACKSIZE) is allocated for the number of tasks created and that there are enough TCBs (MAXNOTASKS)*/
void PCBHeaterTasks::startup(){
	const bool showStats = false;

	init_pos();

	if(!create(taskA,PCBSTACKSIZE,2,showStats))
//	if(!create(taskA,PCBSTACKSIZE,2,false))
		Serial.println(F("Could not create taskA"));

/*
	if(!create(taskB,PCBSTACKSIZE,3,showStats))
		Serial.println(F("Could not create taskB"));

	if(!create(taskC,PCBSTACKSIZE,4,showStats))
		Serial.println(F("Could not create taskC"));
	*/

	if(!start_pos(TRUE)){	// Start POS and allow main system loop to be a task
		Serial.println(F("Unable to start POS"));
		while(1);
	}
}

