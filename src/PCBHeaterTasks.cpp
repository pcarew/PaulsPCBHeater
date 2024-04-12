/*
 * PCBHeaterTasks.cpp
 *
 *  Created on: Apr 3, 2024
 *      Author: PaulCarew
 */

#include "arduino.h"
#include "pos/pos.h"
#include "PCBHeaterTasks.h"
#include "TempDisplay.h"

IMPORT TempDisplay *temps;

	//void (*TaskType)(volatile TCB *);
void PCBHeaterTasks::taskA(volatile TCB *tcb){
	volatile TCB *localTcb = tcb;
	char taskLetter = (char)('A'+tcb->tid - 1);
	char tid = tcb->tid;

	FOREVER{
		Serial.println(F("Task A"));
		temps->update();
		pause();
	}
}

void PCBHeaterTasks::taskB(volatile TCB *tcb){
	volatile TCB *localTcb = tcb;
	char taskLetter = (char)('A'+tcb->tid - 1);
	char tid = tcb->tid;

	FOREVER{
		Serial.println(F("Task B"));
		pause();
	}
}

void PCBHeaterTasks::taskC(volatile TCB *tcb){
	volatile TCB *localTcb = tcb;
	char taskLetter = (char)('A'+tcb->tid - 1);
	char tid = tcb->tid;

	FOREVER{
		Serial.println(F("Task C"));
		pause();
	}
}

#define PCBSTACKSIZE	(unsigned)120 //95 //110 //176 //240

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

