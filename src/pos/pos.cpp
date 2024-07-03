
/************************************************************************
*                                                                       *
*       FILE NAME       : pos.c                                         *
*       REVISION        : 1                                             *
*       DESCRIPTION     : Paul's Operating System source.		*
*                       :
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : pos.h						*
*       NOTE            :                                               *
*                                                                       *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
*               Change History.                                         *
*                                                                       *
*       Rev.    Name    Date            Reason                          *
*                      (MON/DD/YY)                                      *
*                                                                       *
*       1       AJPC    feb/12/93       Initial software                *
*                                                                       *
************************************************************************/
#include <Arduino.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "pos.h"
#include "posTime.h"


IMPORT	unsigned char	no_tasks;		/* Total number of tasks.	*/
IMPORT	unsigned char	no_rtasks;		/* Number of running tasks	*/
IMPORT	TCB	*currt;			/* Current running task		*/
IMPORT	TCB	*nextt;			/* Next task to run 		*/

IMPORT	unsigned int	stack_used;	/* Amount of stack space allocated*/
IMPORT	unsigned char	*sys_sp;
IMPORT	unsigned char	stacks[];
IMPORT	unsigned long	switches;
IMPORT	volatile TCB	tasks[];
IMPORT	TCB 			*systemTcb;

					/* Swap stacks of tasks.	*/
//IMPORT	int	swap_s();
IMPORT "C" { unsigned int  swap_s(unsigned char **csp, unsigned char **nsp);}

//IMPORT	void	noscheck();		/* Defeat Microsoft stack check	*/
/************************************************************************
* Function	: create
* Description	:
************************************************************************/
TCB *create(TaskType task, unsigned int reqStackSize, int param, BOOL showStats)
{
	TCB	*tcb;
	unsigned char	*s = NULL;


	if(no_tasks >= MAXNOTASKS){
		Serial.println(F("Maximum number of tasks exceeded"));
		return FALSE;
	}

	tcb = &tasks[no_tasks];

//		Serial.print(F("New TaskId:")); Serial.print((unsigned int)no_tasks+1,DEC);
//		Serial.print(F(" TCB:")); Serial.println((unsigned int)tcb,HEX);


	if(reqStackSize > 0){				// A stack was requested for this task (this is the default)
		if((stack_used + reqStackSize) > (STACKSIZE)){
				Serial.println(F("Out of stack space for task ")); Serial.println(no_tasks);
				return FALSE;
		}

#ifdef STACK_CHECKS
		tcb->se		= (stacks+stack_used); // Set up Sentry using stack used prior to this task
		*tcb->se	= STACKGUARD;
		tcb->showStats	= showStats;
#endif
		stack_used	+= (reqStackSize+1);
							// stack s ptr always points to next avail location
		s	= (unsigned char *)( (unsigned char *)stacks+stack_used - 1);
//			Serial.print(F("New SP:"));Serial.println((unsigned int)s,HEX);

#ifdef STACK_CHECKS
		tcb->st = s;
//			Serial.print(F(" Stack Base:0x")); Serial.print((unsigned int)stacks,HEX);
//			Serial.print(F(", Stack Size ")); Serial.print(reqStackSize);
//			Serial.print(F(", Bottom:0x")); Serial.print((unsigned int)tcb->se,HEX);
//			Serial.print(F(", Top:0x")); Serial.println((unsigned int)s,HEX);

		// First Fill the stack with a recognizable pattern
		unsigned char *fillPtr = tcb->se+1;		// Skip over the Sentry STACKGUARD
		while(fillPtr <= s){
			*fillPtr++ = (unsigned char) (WATERMARK)+ (no_tasks);		// Fill pattern contains Watermark and Task ID
		}
		tcb->lowtide= reqStackSize;
#endif
		*s--	= lowByte((unsigned int)tcb);	// Store for 16bit int parameter
		*s--	= highByte((unsigned int)tcb);
		*s--	= (unsigned char) lowByte((unsigned int)pos_trap);	// Trap if task exits.
		*s--	= (unsigned char) highByte((unsigned int)pos_trap);	// Trap if task exits.
		*s--	= (unsigned char) lowByte((unsigned int)task);		// Task initial startup
		*s--	= (unsigned char) highByte((unsigned int)task);		// Task initial startup
		s -= 18;	//Register storage reservation
//		s -= 6;		//Compiler added function preamble register pushes

		tcb->sp		= s;			// AVR SP always points to next available location
	}else{ // Determine if TCB has a stack - This else indicates that a thread stack was not needed
#ifdef STACK_CHECKS
		tcb->se		= NULL;
		tcb->st		= NULL;
		tcb->showStats	= FALSE;
#endif
		tcb->sp		= NULL;			// A stack wasn't requested, probably because this will be the system task
	}
							// which uses the system stack

//		Serial.print(F("stackSet 0x"));Serial.println((unsigned int)tcb->sp,HEX);
	if(!no_tasks){				// No existing tasks - IE this is the 1st requested task
		nextt	= tcb;
		tcb->next = tcb;		// Point fwd and back to this task
		tcb->prev = tcb;
		no_rtasks++;			// Number of *** Running *** tasks
	}else{
		qtask(tcb);				// Add this subsequent task to the run	queue
	}

	tcb->param = param;				// Set the task param value
	tcb->tid = no_tasks;			// Set the Task Id for debugging - zero referenced

	no_tasks++;

	return tcb;
}

/************************************************************************
* Function	: qtask
* Description	: Adds task to runq, at the end of the runq.
*		: The runq is maintained as a circular list.
************************************************************************/
void qtask(TCB *tcb)
{

	tcb->next	= nextt;
	tcb->prev	= nextt->prev;
	nextt->prev->next= tcb;
	nextt->prev	= tcb;

	no_rtasks++;
}

/************************************************************************
* Function	: dqtask
* Description	: Removes the current task from the runq
************************************************************************/
TCB *dqtask(void)
{
	if(!(--no_rtasks))
		pos_trap();			// Removing the last running task

						// Ensure that nextt is pointing to the next task to run
						/* If nextt points to	 */
	if(nextt == currt)			/* task that we are about*/
		nextt = currt->next;		/* to dq then advance it.*/

						/* Unlink the current	 */
						/* task from the run Q	 */
	currt->prev->next= currt->next;
	currt->next->prev= currt->prev;

	return currt;
}


/************************************************************************
* Function	: start_pos
* Description	: This returns a false if unable to start for any reason.
*		: If a request was made to turn the calling thread into the system task,
*		: then true will be returned, otherwise, this function is not expected to return
************************************************************************/
BOOL start_pos(BOOL system)
{
	unsigned char **csp;
	int result	= 1234;

//	Serial.print(F("Starting Tasking. UnAllocated stack:")); Serial.println((unsigned int) (STACKSIZE - stack_used));

	if(system){
		systemTcb = create((TaskType)NULL,0,0,FALSE);
		if(systemTcb == (TCB *)NULL){
			Serial.println(F("Error no system TCB"));
			return FALSE;
		}else{
//			Serial.print("System Task created with TID:"); Serial.print(systemTcb->tid); Serial.print("\r\n");
		}
	}
	if(!no_tasks){
		Serial.print(F("No tasks created."));
		while(1);//exit(1);
	}
	currt	= nextt;
	nextt	= currt->next;
	csp	= &currt->sp;


					// Start task swapping by switching stacks
	//Serial.print("Starting First Task "); Serial.print(currt->tid); Serial.print("\r\n");
	if(system){
					// This thread becomes the system task. System task created above
//			Serial.print(F("Leaving System Thread &systemTcb-sp: ")); Serial.print((unsigned)&systemTcb->sp,HEX);
//			Serial.print(F(" systemTcb->sp:")); Serial.print((unsigned)systemTcb->sp,HEX);
//			Serial.print(F(" csp:")); Serial.print((unsigned)csp,HEX);
//			Serial.print(F(" *csp:")); Serial.println((unsigned)*csp,HEX);
//			delay(50); //AJPC
		result = swap_s(&(systemTcb->sp),csp);		/* Swap stacks of tasks.	*/

//			Serial.print(F("Result for starting POS with system task ")); Serial.println((unsigned)result);
	}else{
//			Serial.print("Leaving System Thread &sys_sp: "); Serial.print((unsigned)&sys_sp,HEX);
//			Serial.print(" sys_sp:"); Serial.print((unsigned)sys_sp,HEX);
//			Serial.print(" csp:"); Serial.print((unsigned)csp,HEX);
//			Serial.print(" *csp:"); Serial.print((unsigned)*csp,HEX);
//			Serial.print("\r\n");

		result = swap_s(&sys_sp,csp);		/* Swap stacks of tasks.	*/
					/* sys_sp is used to hold the	*/
					/* system stack pointer.	*/

			Serial.print(F("Panic ")); Serial.println((unsigned)result);		/* We should never get here!	*/
			Serial.print(F("Ending pos &sys_sp: ")); Serial.print((unsigned)&sys_sp,HEX);
			Serial.print(F(" sys_sp:")); Serial.print((unsigned)sys_sp,HEX);
			Serial.print(F(", csp:")); Serial.print((unsigned)csp,HEX);
			Serial.print(F(" *csp:")); Serial.print((unsigned)*csp,HEX);
		while(1) delay(100); //exit(1);
	}
//			Serial.print(F("System Task continuing"));
//			Serial.print(F(" &sys_sp: ")); Serial.print((unsigned)&sys_sp,HEX);
//			Serial.print(F(" sys_sp:")); Serial.print((unsigned)sys_sp,HEX);
//			Serial.print(F(", csp:")); Serial.print((unsigned)csp,HEX);
//			Serial.print(F(" *csp:")); Serial.println((unsigned)*csp,HEX);
	return TRUE;
}

/************************************************************************
* Function	: pause
* Description	: Casues control to pass to the next task in the runq
************************************************************************/
void pause()
{
	unsigned char	**csp;
	unsigned char	**nsp;
	int		result;
	CLR_INTR();			/* Interrupt sensitive.		*/
#ifdef STACK_CHECKS
		//Serial.print("\nPausing TID:"); Serial.print(currt->tid);
		//Serial.print(" curr se"); Serial.print(currt->se);
		//Serial.print(" next se:"); Serial.print(nextt->se);
			//Serial.print("\r\n");
			// Only Guard check stacks that belong to POS
	if( !stack_check(currt,currt->showStats)){
		SET_INTR();
		while(1){
			Serial.print(F("Stack polluted for pausing task ")); Serial.println(currt->tid);delay(1000);
		}
	}else{
		//Serial.print("Stack OK for pausing task "); Serial.print(currt->tid); Serial.print("\r\n");
	}

	if( !stack_check(nextt,nextt->showStats)){
		SET_INTR();
		while(1){
			Serial.print(F("Stack polluted for resuming task ")); Serial.println(nextt->tid);delay(1000);
		}
	}else{
//		Serial.print("Stack OK for resuming task "); Serial.print(nextt->tid); Serial.print("\r\n");
	}
#endif
	switches++;			/* Used for performance mon.	*/

	csp	= &currt->sp;		/* Get location for the	SP of	*/
					/* the current task.		*/
	nsp	= &nextt->sp;		/* And the next..		*/

	currt	= nextt;
	nextt	= nextt->next;

	SET_INTR();

		//Serial.print("Calling swap pos csp:%u *csp:%u\n",(unsigned)csp,(unsigned)*csp);
		//Serial.print("                 nsp:%u *nsp:%u\n",(unsigned)nsp,(unsigned)*nsp);
	result = swap_s(csp,nsp);		/* Swap stacks of tasks.	*/
		//Serial.print("\n Resuming TID:%d Result:%d\n",currt->tid,result);
}
/************************************************************************
* Function	: init_pos
* Description	: Initialize all POS global variables and setup POS
*		  environment.
************************************************************************/
void init_pos()
{
					/* Init POS global task related	*/
					/* variables.			*/
	currt	= (TCB *)NULL;
	nextt	= (TCB *)NULL;

	no_tasks= (char)NULL;
	no_rtasks=(char)NULL;

	stack_used	= (int)NULL;
	switches	= (unsigned long)NULL;

#ifdef POSTIME
	pos_inittime();			/* Initialize timer services.	*/
#endif
					/* Setup POS environment.	*/
	//noscheck();			/* Turn off microsoft stack	*/
					/* checking.			*/
//	Serial.println(F("Finished initializing POS"));
}

/************************************************************************
* Function	: postrap
* Description	: This func. is called if an unrecoverable error has occured
*		  A pointer to this function is placed at the top of the
*		  stack for all tasks. If a task terminates, which is
*		  illegal, this function will be called.
************************************************************************/
void pos_trap()
{
	Serial.print(F("\r\nPOS trap, unrecoverable error. TaskId: ")); Serial.print(currt->tid);
	while(1);//exit(1);
}

#ifdef STACK_CHECKS
static	unsigned char *sc_p;
static	unsigned int sc_lowtide = 0;
static	unsigned int sc_stackSize;
BOOL stack_check(TCB *tcb,BOOL show)
{
	if( tcb->se != NULL && *tcb->se	!= STACKGUARD){
		Serial.println(F("Error Task "));// Serial.print(tcb->tid);
		Serial.println(F(" StackGuard failure (overrun)"));
		return FALSE;
	}

	if(tcb->st != NULL && tcb->se != NULL && tcb->sp != NULL){
		sc_lowtide = 0;
		sc_stackSize = tcb->st - tcb->se;
		for(sc_p=tcb->se+1; sc_p < tcb->sp; sc_p++){
			if(*sc_p != WATERMARK+tcb->tid){
				break;
			}else{
				sc_lowtide++;
			}
		}
		tcb->lowtide = sc_lowtide;
		if(show == TRUE){
			Serial.print(F("Stack for TID:")); Serial.print(tcb->tid);
			Serial.print(F(" has ")); Serial.print(sc_lowtide);
			Serial.print(F(" untouched space, which is ")); Serial.print(((sc_lowtide*100)/sc_stackSize));
			Serial.print(F("% of ")); Serial.println(sc_stackSize);
		}
	}
	return TRUE;
}
#define POSPERIOD (1000)
//#define POSHZSCALE  (1000.0/(double)POSPERIOD)
int pos_stats(){	// return number of task switches per second
	static unsigned long periodEnd = 0;
	static int hz = 0;
	if(millis() > periodEnd){
		periodEnd = millis()+POSPERIOD;
//		hz = (int)((double)switches * POSHZSCALE);
		hz = (int)(switches );
		switches = 0;
	}
	return hz;
}
void chack_all_tcb(){
	TCB *tcb;
	tcb = nextt;
	do {
		if(!stack_check(tcb,false)){
			Serial.print(F("TASK Stack Guard Fail. TaskId:"));Serial.println(tcb->tid);delay(10);
			while(1);
		}
		if(tcb->lowtide<20){
			Serial.print(F("Task Low Stack TaskId:"));Serial.print(tcb->tid);Serial.print(F(" Remaining: "));Serial.println(tcb->lowtide);
		}
		tcb = tcb->next;
	}while (nextt != nextt->next);
}
#endif
