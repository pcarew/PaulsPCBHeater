/*
 * PCBHeaterTasks.h
 *
 *  Created on: Apr 3, 2024
 *      Author: PaulCarew
 */

#ifndef PCBHEATERTASKS_H
#define PCBHEATERTASKS_H
#include "pos/pos.h"

/*
 * Reason to have separate tasks:
 * 	A task could be suspended, pending on some outside (of the task) event, thus not taking processing cycles
 * 	A task could require different amount of stack space
 * 	A task might have a different run schedule/priority
 *
 * 	Tasks may also invoke 'threads' in other classes. A Thread is NOT separately scheduled and is invoked from  within the a task
 */
class PCBHeaterTasks {
public:
	static void startup();

	//void (*TaskType)(volatile TCB *);
	static void taskA(volatile TCB *);
	static void taskB(volatile TCB *);
	static void taskC(volatile TCB *);
};

#endif /* PCBHEATERTASKS_H_ */
