
/************************************************************************
*                                                                       *
*       FILE NAME       : posMsg.c                                      *
*       REVISION        : 1                                             *
*       DESCRIPTION     : Paul's Operating System source.		*
*                       : Inter-task messaging.
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : pos.h,posMsg.h				*
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
#include "arduino.h"
#include "pos.h"
#include "posMsg.h"

/************************************************************************
* Function	: init_mbox
* Description	: This function initializes a mailbox so that it is ready
*		  for use.
************************************************************************/
void init_mbox(MBOX *mbox)
{
	mbox->msg    = (MSG)NULL;
	mbox->pended = (TCB *)NULL;
}

/************************************************************************
* Function	: post
* Description	: This function posts a message to a mailbox. If a task
*		  is pending for a message on this mailbox, then the task
*		  is put back in the runq.
************************************************************************/
BOOL post(MBOX *mbox,MSG msg)
{
	if(mbox->msg){			/* If a msg. is already posted	*/
		return FALSE;		/* then return NOT POSTED (NULL)*/
	}

	mbox->msg = msg;
					/* If a task was waiting for this*/
	if(mbox->pended){		/* then requeue it on the run Q	*/
		CLR_INTR();
		qtask(mbox->pended);
		SET_INTR();
		mbox->pended = (TCB *)NULL;
	}
}

/************************************************************************
* Function	: pend
* Description	: This function obtains a message from a mailbox. If
*		  there is not a message waiting in the mailbox, then
*		  the task is removed from the runq until one arrives.
************************************************************************/
MSG pend(MBOX *mbox)
{
	MSG	msg;

	if(!mbox->msg){
					/* Suspend task until something	*/
					/* is posted to this mail box,	*/
					/* providing no other tasks are	*/
					/* already pending.		*/
		if(!mbox->pended){
			CLR_INTR();
			mbox->pended = dqtask();
			SET_INTR();
			pause();	/* Cause reschedule.		*/
		}else{
			return (MSG)NULL;
		}
	}
					/* Something here, therefore	*/
					/* return it.			*/
	msg = mbox->msg;
	mbox->msg = (MSG)NULL;
	return msg;
}
