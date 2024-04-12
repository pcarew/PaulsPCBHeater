/*
 * PCBHeaterTasks.h
 *
 *  Created on: Apr 3, 2024
 *      Author: PaulCarew
 */

#ifndef PCBHEATERTASKS_H
#define PCBHEATERTASKS_H
#include "pos/pos.h"

class PCBHeaterTasks {
public:
	static void startup();

	//void (*TaskType)(volatile TCB *);
	static void taskA(volatile TCB *);
	static void taskB(volatile TCB *);
	static void taskC(volatile TCB *);
};

#endif /* PCBHEATERTASKS_H_ */
