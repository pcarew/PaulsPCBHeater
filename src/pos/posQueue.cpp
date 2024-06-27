/************************************************************************
*                                                                       *
*       FILE NAME       : posQueue.cpp					*
*       REVISION        : 1                                             *
*       DESCRIPTION     : Type definitions for Queues in Paul's Operating*
*			: System. Derived from original OS by PC.	*
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : None
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
*       1       AJPC    may/14/24       Initial software                *
*                                                                       *
************************************************************************/
#include "arduino.h"
#include "pos.h"
#include "posQueue.h"

/************************************************************************
* Function	: init_queue
* Description	: This function Initializes a new Inter or Intra task MSG queue
* 				It wants a Queue object and an array of MSG to use
*		  is put back in the runq.
************************************************************************/
void init_queue(QUE *q, MSG *qStack, unsigned char size){
	q->size = size;
	q->qPtr = 0;
	q->queue = (qStack == NULL?new MSG[size]:qStack);
	q->pended = (TCB*)NULL;
}
/************************************************************************
* Function	: qPush
* Description	: This function queues up a msg if there is room
*				If a  task is pending for a message on this queue, then the task
*		  		is put back in the runq.
************************************************************************/
bool qPush(QUE *q, MSG *msg){
	if(q->qPtr < q-> size){
		q->queue[q->qPtr++] = msg;

		if(q->pended){		/* A task was pending, so requeue it on the run Q	*/
			CLR_INTR();
			qtask(q->pended);
			SET_INTR();
			q->pended = (TCB *)NULL;
		}
		return true;
	}else{
		return false;
	}
}
/************************************************************************
* Function	: qPop
* Description	: This function obtains a message from the queue. If
*		  there is not a message waiting in the queue (and pendRequested was set), then
*		  the task is removed from the runq until one arrives.
************************************************************************/
MSG qPop(QUE *q, bool pendRequested){
	MSG	msg;

	if(q->qPtr <= 0 ){
		if(pendRequested){
						/* Suspend task until something	*/
						/* is posted to this queue,	*/
						/* providing no other tasks are	*/
						/* already pending.		*/
			if(!q->pended){
				CLR_INTR();
				q->pended = dqtask();		// Remove this Task from RunQueue and place here for safe keeping
				SET_INTR();
				pause();					// Cause reschedule. Will return when something is added to Q
											// Msg was was added while we were pended, causing us to run again
			}
		} else{
			return (MSG)NULL;				// Nothing in Q and no request to pend received, so just return empty handed
		}
	}
						/* Something here, therefore	*/
						/* return it.			*/

	msg = q->queue[--q->qPtr];			// Either something was originally here or it was added while we were pended
	return msg;
}
/************************************************************************
* Function	: qCheck
* Description	: This function returns the number of entries on the queue
************************************************************************/
int qCheck(QUE *q){
	return q->qPtr;
}

/************************************************************************
* Function	: qWipe
* Description	: This function clears the queue. Careful. If any of the msgs
* 				were malloc'd, this may cause a memory leak unless 'useFree'
* 				is specified.
************************************************************************/
void qWipe(QUE *q, bool useFree){
	if(useFree){
		while(q->qPtr){
			MSG msg = q->queue[--q->qPtr];
			free(msg);
		}
	}
	q->qPtr  = 0;
}
