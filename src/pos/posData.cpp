
/************************************************************************
*                                                                       *
*       FILE NAME       : posdata.c                                     *
*       REVISION        : 1                                             *
*       DESCRIPTION     : Paul's Operating System data definitions.	*
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
#include "pos.h"
#include "posTime.h"

unsigned char	no_tasks = 0;			/* The number of total tasks.	*/
unsigned char	no_rtasks = 0;			/* The number of running tasks.	*/

TCB	*currt;				/* Identifies the currently	*/
					/* running task.		*/
TCB	*nextt;				/* Identifies the next task to	*/
					/* run.				*/
					/* The run queue is in fact a	*/
					/* circular doubly linked list	*/
					/* of running TCBs. The variable*/
					/* 'currt' is updated to always	*/
					/* point to the current running	*/
					/* task. The variable 'nextt'	*/
					/* is updated to always point to*/
					/* the next task to run.	*/

unsigned char	*sys_sp;		/* The original system stack	*/
								/* pointer.			*/
TCB *systemTcb	= (TCB *)NULL;	/* Used if system task used 'see pos_start()'*/

unsigned int	stack_used;		/* The amount of stack allocated*/
					/* so far.			*/
					/* The stacks.			*/
unsigned char stacks[STACKSIZE];		// Any task that uses printf library
					// call needs at least 500 words
					// of stack space

unsigned long	switches;		/* Running count of the number of*/
					/* task switches.		*/

TMR	timerq;				/* Points to the timer queue.	*/
					/* This is a linked list of	*/
					/* timers that are running for	*/
					/* a specified time. This is an	*/
					/* ordered list with the timer	*/
					/* that will timeout soonest at	*/
					/* head. 'timerq' is actually a */
					/* dummy TMR variable. This is	*/
					/* used instead of a TMR pointer*/
					/* because it simplifies the 	*/
					/* insertion code.		*/
TMR	*timerend;			/* Points to the last entry on	*/
					/* the timer q. This is needed	*/
					/* because the TMR to be placed	*/
					/* in the queue, is initially	*/
					/* thrown on the end of the 	*/
					/* linked list as a 'sentinal'.	*/
POS_TMR pos_timer;			/* This is the POS system	*/
					/* Timer. It holds a count of	*/
					/* The number of times the H/W	*/
					/* timer should run with a whole*/
					/* count and the size of a 	*/
					/* partial count. i.e. the pos	*/
					/* timer will run for:		*/
					/* (wc*256+pc)*1/12osc.		*/
unsigned long posclock = 0;			/* System clock;		*/
volatile TCB tasks[MAXNOTASKS];			// Holds Task Control Blocks
volatile TMR timers[MAXNOTIMERS];

/*
#include <stdio.h>
void info()
{
	Serial.print("stacks is: "); Serial.print((unsigned int)stacks); Serial.print(", Size is "); Serial.print(sizeof(stacks)); Serial.print("\r\n");
	Serial.print("Tasks  is: "); Serial.print((unsigned int)tasks); Serial.print(", Size is "); Serial.print(sizeof(tasks)); Serial.print(" No is "); Serial.print(sizeof(tasks)/sizeof(TCB)); Serial.print("\r\n");

	Serial.print("Timers is: "); Serial.print((unsigned int)timers);
	Serial.print(", Size is "); Serial.print(sizeof(timers));
	Serial.print(" No is "); Serial.print(sizeof(timers)/sizeof(TMR)); Serial.print("\r\n");
}
*/
