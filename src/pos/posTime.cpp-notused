
/************************************************************************
*                                                                       *
*       FILE NAME       : postime.c                                     *
*       REVISION        : 1                                             *
*       DESCRIPTION     : Paul's Operating System source.		*
*                       : Timer services
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
*       1       AJPC    feb/13/93       Initial software                *
*                                                                       *
************************************************************************/
#include <Arduino.h>
//#include <stdio.h>
#include <stdlib.h>
#include "pos.h"
#include "posTime.h"

IMPORT	unsigned long	posclock;
IMPORT	POS_TMR		pos_timer;
IMPORT	TMR		timerq;
IMPORT	TMR		*timerend;
IMPORT	TMR		timers[];;

IMPORT	unsigned char GET_HW_TIME(void);
IMPORT	void	LOADTIMER(unsigned char);
IMPORT	void	start_hwtimer(void);
IMPORT	void	chk_timerq(void);
IMPORT	void	qtimer(TMR *);
IMPORT	void	setup_systimer(unsigned long);
IMPORT	void	restart_systimer(unsigned long);


static TMR *getTmr();
static void freeTmr(TMR *tmr);

/************************************************************************
* Function	: pos_inittime
* Description	: Initailize all timer related variables and start system
*		: timer going.
************************************************************************/
void pos_inittime(void)
{
	int i;
	for(i=0;i<MAXNOTIMERS;i++){
		timers[i].tmr_type = UNUSED;
	}
	
	timerq.next	= (TMR *)NULL;
	timerend	= (TMR *)NULL;
	setup_systimer(DEFAULTTIME);
	start_hwtimer();
}

/************************************************************************
*	Application timer routines.					*
************************************************************************/

/************************************************************************
* Function	: timer
* Description	: Creates an application timer of 'time' length. This is more
*		: accurate than delaying a task because a task that has
*		: an expired 'delay timer' is scheduled to run and thus will
*		: experience an additional delay due to the other running
*		: tasks.
*		: A timer executed function, however, is extremely limited in
*		: the amount of processing that can be done as it will be
*		: running as part of an interrupt service routine. For this
*		: reason, timers should only be used, if a task delay is not
*		: suitable and then the processing must be kept to a minimum.
*		: A timer executed function, also, must NOT call any POS
*		: functions that cause a task swap. Because the timer
*		: executed function is running as part of the timer interrupt
*		: service routine, it cannot start any new timers.
************************************************************************/
TMR *timer(unsigned long timeTicks, void(*func)(int),int param)
{
	TMR *tmr;
	if(!(tmr = (TMR *)getTmr()) ) return (TMR *)NULL;
	tmr->tmr_type = INTVTIMER;
	tmr->func = func;
	tmr->param= param;
	tmr->timeout = posclock + timeTicks;

	qtimer(tmr);

	return tmr;
}

/************************************************************************
* Function	: cancel
* Description	: Cancels previously started interval timer.
*		: The cheapest way to do this in terms of terms of
*		: processing and code space required, is to simply leave
*		: the timer in the timer queue but mark it as cancelled.
*		: The time will actually be removed by the normal timer
*		: code when its timeout expires. No validity checking is
*		: performed.
************************************************************************/
void cancel(TMR *tmr)
{
	tmr->tmr_type = CANCELLED;
}
/************************************************************************
* Function	: delay
* Description	: Delays task for 'time' clock ticks. The delayed task
*		  is taken off the runq and placed on a delayed task queue
************************************************************************/
IMPORT TCB *currt;
BOOL posDelay(unsigned long timeTicks)
{
	TMR *tmr;
	if(!(tmr = (TMR *)getTmr()) ){
		while(1);
		return FALSE;
	}
	tmr->timeout	= (unsigned long)(posclock + timeTicks);
	tmr->tmr_type	= TASKDELAY;
	CLR_INTR();
	tmr->tcb	= dqtask();
	SET_INTR();

	/*
	Serial.print(F("Delaying until ")); Serial.print(tmr->timeout);
	Serial.print(F(" posclock:")); Serial.print(posclock);
	Serial.print(F(" timeticks:")); Serial.print(timeTicks);
	Serial.print(F(" TO: ")); Serial.println((posclock+timeTicks));
	*/

	qtimer(tmr);

	//Serial.print("Delay for task %d:  after 'q add' until %lu posclock:%lu timeticks:%lu TO: %lu\n",currt->tid,tmr->timeout,posclock,timeTicks,(posclock+timeTicks));
	//Serial.println(F("Pausing"));
	pause();			/* Cause reschedule.	*/

	return TRUE;
}

/************************************************************************
* Function	: qtimer
* Description	: This function queues a timer onto the timer queue in
*		: the correct position. The timer queue is an ordered linked
*		: list that is arranged in timer expiration.
************************************************************************/
void qtimer(TMR *tmr)
{
	TMR *cur;

	tmr->next	= (TMR *)NULL;

	CLR_INTR();				/* Interrupt sensitive	*/

	if(!timerq.next){
			//Serial.print("first tmr\n");
		timerq.next	= tmr;
		timerend	= tmr;
		restart_systimer(tmr->timeout-posclock);
	}else{
			//Serial.print("adding tmr to list\n");
		timerend->next	= tmr;		/* Throw tmr on end of	*/
						/* list as a sentinal.	*/

						/* Walk down list until	*/
						/* correct position is	*/
						/* is found. This will	*/
						/* not fall off the end	*/
						/* of the list because	*/
						/* of the sentinal.	*/
						/* 'timerq' is a dummy	*/
						/* TMR structure that	*/
						/* enables us to avoid	*/
						/* special conditional	*/
						/* processing at the	*/
						/* head of the queue.	*/
		cur = &timerq;
		while(tmr->timeout > cur->next->timeout){
			cur = cur->next;
		}

						/* If we reached the end*/
						/* of the list, then the*/
						/* new TMR can stay on	*/
						/* on the end, thus set	*/
						/* new end pointer.	*/
		if(cur == timerend){
			timerend = tmr;
		}else{
						/* TMR belongs in the	*/
						/* middle. Link it in	*/
						/* and remove sentinal	*/
			tmr->next = cur->next;
			cur->next = tmr;
			timerend->next  = (TMR *)NULL;
						/* If tmr was placed at	*/
						/* head of queue the	*/
						/* readjust the HW timer*/
			if(cur==&timerq){
					//Serial.print("Restarting systimer\n");
				restart_systimer(tmr->timeout-posclock);
			}
		}
	}
	SET_INTR();
}


/************************************************************************
*	POS System timer routines.					*
************************************************************************/

/************************************************************************
* Function	: setup_systimer
* Description	: Initialize POS system timer.
************************************************************************/
void setup_systimer(unsigned long time)
{
	pos_timer.wc = (unsigned char)(time / HWTIMERSIZE);
	pos_timer.pc = (unsigned char)(time % HWTIMERSIZE);
	pos_timer.active = TRUE;
}

/************************************************************************
* Function	: restart_systimer
* Description	: This function restarts the POS system timer before it
*		: has expired. The length of time that the hardware timer
*		: has been running is added to the system clock. The new
*		: timer length is then setup in the system timer and the
*		: hardware timer started.
************************************************************************/
void restart_systimer(unsigned long time)
{
	//Serial.print("Restart old posclock:%lu GetHWTime: %lu pos_timer:%u HWTIMERSIZE:%lu\n",posclock,GET_HW_TIME(),(unsigned char)pos_timer.inc,HWTIMERSIZE);
	posclock += (unsigned long)(GET_HW_TIME()+pos_timer.inc-HWTIMERSIZE);
	//Serial.print("Restart New posclock: %lu\n",posclock);
	setup_systimer(time);
	start_hwtimer();
}

/************************************************************************
* Function	: start_hwtimer
* Description	: This function starts the hardware timer with either a
*		: full hardware clock count or a partial one.
************************************************************************/
void start_hwtimer(void)
{
	if(pos_timer.wc){
		pos_timer.wc--;
		pos_timer.inc = (unsigned char)HWTIMERSIZE;
		LOADTIMER(0);
	}else{
		pos_timer.inc = (unsigned char)pos_timer.pc;
		LOADTIMER(HWTIMERSIZE - pos_timer.pc);
		pos_timer.active = FALSE;			// Using up the partial count so timer now exhausted
	}
}


/************************************************************************
*	Hardware timer routines.					*
************************************************************************/


/************************************************************************
* Function	: timerisr
* Description	: This function is called when the hardware timer expires
*		: causing an interrupt. If the system timer is still
*		: active then the hardware timer is reloaded. If the system
*		: timer is no longer active, then the hardware timer is
*		: restarted with a default timer time so that it is running
*		: while the application timers are being checked for
*		: expirations.
************************************************************************/
void timerisr(void)
{
	unsigned long time;
	//putchar('.');
	//Serial.print(F("TISR:")); Serial.println((pos_timer.active?"Active":"InActive"));

	posclock += (unsigned long)pos_timer.inc;
	if(!pos_timer.active){
		setup_systimer(DEFAULTTIME);	/* Get H/W timer going	*/
		start_hwtimer();		/* again.		*/
		chk_timerq();			/* Now process expired	*/
						/* timers.		*/
		time = (timerq.next)?
				(timerq.next->timeout-posclock)
			:
				DEFAULTTIME;
		restart_systimer(time);
						/* Now restart H/W timer*/
						/* with user timer.	*/
	}else
		start_hwtimer();
}

/************************************************************************
* Function	: chk_timerq
* Description	: This is called from the timer ISR. It checks the timeq
*		: for expired timers. If the timer was for a task delay,
*		: then the task is rescheduled.
*		: If the timer was an interval timer then the relevent
*		: function function is called.
************************************************************************/
void chk_timerq()
{
	TMR	*tmr;

	//Serial.print("chk_timerq posclk: %lu, timerq.next:%s t/o:%lu\n",posclock,(timerq.next?"true":"false"),timerq.next->timeout);
	while(timerq.next && (posclock >= timerq.next->timeout) ){
						/* Unlink timer		*/
		tmr = timerq.next;
		timerq.next = tmr->next;
		if(!timerq.next)
			timerend = &timerq;
						/* process timer	*/
		switch(tmr->tmr_type){
			case INTVTIMER: (*tmr->func)(tmr->param);
					break;
			case TASKDELAY: qtask(tmr->tcb);
					break;
			case CANCELLED:		/* Do nothing.		*/
					break;
			case UNUSED:
					Serial.println(F("We found an unused timer on the chain"));
					pos_trap();
					break;
			default:	pos_trap();
					break;
		}
		freeTmr(tmr);
	}
}
TMR *getTmr()
{
	TMR *tmr;
	int i;
	for(i=0;i<MAXNOTIMERS;i++){
		tmr  = &timers[i];
		if(tmr->tmr_type == UNUSED){
			//Serial.print(F("Supplying tmr ")); Serial.println(i);
			return tmr;
		}
	}
	Serial.print(F("\nNo Timers available"));
	return (TMR *) NULL;

	/*
	if(!(tmr = (TMR *)malloc(sizeof(TMR))) )
		return (TMR *)NULL;
	else
		return tmr;
		*/

}
void freeTmr(TMR *tmr)
{
	tmr->tmr_type	= UNUSED;
	tmr->timeout	= 0;
	//free(tmr);
}

