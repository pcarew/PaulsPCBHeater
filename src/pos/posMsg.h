/************************************************************************
*                                                                       *
*       FILE NAME       : posmsg.h					*
*       REVISION        : 1                                             *
*       DESCRIPTION     : Type definitions for Paul's Operating		*
*			: System. Intertask messaging.			*
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
typedef void *MSG;			/* POS message type.		*/

typedef	struct {			/* POS mail box type		*/
	MSG	msg;
	TCB	*pended;
}MBOX;

IMPORT	BOOL	post(MBOX *mbox,MSG msg);
IMPORT	MSG		pend(MBOX *mbox);
IMPORT	void	init_mbox(MBOX *mbox);

#define CHK_MBOX(mbox)	(mbox->msg?TRUE:FALSE)
#define GET_MSG(mbox)	(CHK_MBOX(mbox)?pend(mbox):FALSE)
