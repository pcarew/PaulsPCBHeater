/************************************************************************
*                                                                       *
*       FILE NAME       : PosTimingSupport.c                            *
*       REVISION        : 1                                             *
*       DESCRIPTION     : Paul's Operating System source.		*
*                       : timing support
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : pos.h,postime.h				*
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
//#include <dos.h>
#include <Arduino.h>
//#include <stdio.h>
#include <stdlib.h>
#include "pos.h"
#include "posTime.h"

unsigned char u8HWTimer;			// Limited to 255
extern long long current_timestamp() ;
long long  (*old_tmr)();
void timertask(int);
void pos_hwtmr();



/************************************************************************
* Function	: 
* Description	:
************************************************************************/
void posTimingInit(void)
{
	if(!create(timertask,150,1,FALSE))
	{
		Serial.print("Can't start timer task\r\n");
		while(1);//exit(1);
	}else{
		Serial.print("Timer task created\r\n");
	}

}

/************************************************************************
* Function	: pos_hwtmr
* Description	: PC interrupt service routine for fake timer
************************************************************************/
void pos_hwtmr()
{
	//(*old_tmr)();			/* Perform any original actions */

	u8HWTimer++;
//Serial.print("hwtimer:%d\n",hwtimer);
	if(!u8HWTimer) timerisr();
}


/************************************************************************
* Function	: LOADTIMER
* Description	: Setup PC fake hw timer.
************************************************************************/
void LOADTIMER(unsigned char count)
{
	u8HWTimer=count;
}

/************************************************************************
* Function	: GET_HW_TIMER
* Description	: get PC fake hw timer value.
************************************************************************/
unsigned char GET_HW_TIME(void)
{
	return u8HWTimer;
}

/*
#include <sys/time.h>

long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // Serial.print("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
*/
long long current_timestamp() {
	//static long long count = 0;
	//return count++;
	return millis();
}
IMPORT TCB *currt;
void timertask(int param)
{
	static long long prevTimestamp = 0;
	long long currTimestamp;
	int delta;
	int i;

	prevTimestamp = current_timestamp();			// Prime it
	Serial.print(F("Starting up timertask TID:"));Serial.println(currt->tid);
	FOREVER{
		currTimestamp = current_timestamp();
		delta = currTimestamp - prevTimestamp;
		prevTimestamp = currTimestamp;
			//Serial.print(F("Value:")); Serial.print((unsigned long)currTimestamp); Serial.print(F(" Delta: ")); Serial.println(delta);
		for(i=0;i<delta;i++){
			pos_hwtmr();		// Pulse the timer for each millisecond that elapsed
			pause();
		}
		pause();
	}
}
