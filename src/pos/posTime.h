/************************************************************************
*                                                                       *
*       FILE NAME       : postime.h					*
*       REVISION        : 1                                             *
*       DESCRIPTION     : Type definitions for Paul's Operating		*
*			: System. Timer services			*
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : pos.h
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
					/* POS Timer definitions.	*/
#define	ONESECOND	(unsigned long)1000L		// Number of ticks in 1 second
#define FIVESECOND	(5*ONESECOND)
#define TENSECOND	(2*FIVESECOND)
#define ONEMINUTE	(6*TENSECOND)
#define ONEHOUR		(60*ONEMINUTE)

#define	DEFAULTTIME	FIVESECOND

				/** Application timers.			**/
#define INTVTIMER	0		/* Timer type			*/
#define TASKDELAY	1		/* Timer type			*/
#define	CANCELLED	2		/* Cancelled interval timer.	*/
#define UNUSED		3		// It is not in the timer chain

typedef struct _tmr {
	unsigned long	timeout;	/* Absolute time in timer ticks.*/
	char	tmr_type;		/* Task delay or interval timer	*/
	struct _tmr *next;		/* next timer in queue.		*/
	TCB	*tcb;			/* Used if task delay		*/
	void	(*func)(int);		/* Used if interval timer.	*/
	int	param;			/* Used if interval timer.	*/
}TMR;

				/** The POS system timer.		**/
					/* The POS timer is the current */
					/* running timer expressed in	*/
					/* increments of the hardware	*/
					/* timer. i.e. length of POS timer*/
					/* = ((wc*256)+pc)*1/12osc	*/
					/* where 'osc' is proc. crystal	*/
					/* oscilator frequency.		*/
typedef struct {
	unsigned char wc;			/* Number of 256 whole counts to run	*/
	unsigned char pc;			/* Size of partial count.	*/
	unsigned char inc;			/* Current hw timer timeout	*/
	BOOL active;			/* Indicates whether system	*/
}POS_TMR;				/* timer has expired.		*/

#define MAXNOTIMERS 5
#define HWTIMERSIZE 255
					/* POS Function prototypes.	*/

IMPORT	TMR	*timer(unsigned long clockTicks, void(*func)(),int param);
IMPORT	void	cancel(TMR *tmr);
IMPORT	BOOL	posDelay(unsigned long clockTicks);
IMPORT	void	posTimingInit(void);
IMPORT	void	pos_inittime(void);
IMPORT	void	timerisr(void);

