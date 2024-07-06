/************************************************************************
*                                                                       *
*       FILE NAME       : pos.h						*
*       REVISION        : 1                                             *
*       DESCRIPTION     : Type definitions for Paul's Operating		*
*			: System. Derived from original OS by PC.	*
*       AUTHOR          : A.J.Paul Carew                                *
*       FILE ORIGIN     : Original work					*
*       DEPENDENCIES    : None.
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
#ifndef POS_H
#define POS_H
					/*Basic POS types & definitions*/
typedef	unsigned char	BOOL;
#define	FOREVER for(;;)
#define	IMPORT	extern
#ifdef NULL
#undef NULL
#endif
#define	NULL	0
#define	FALSE	((BOOL)0)
#define	TRUE	!FALSE

#define STACK_CHECKS TRUE

#define STACKSIZE	(unsigned)185 // enough for 1 thread task and 1 system task that uses it's own stack
#define	MAXNOTASKS	2


typedef struct _tcb{			/* POS task control block.	*/
#ifdef STACK_CHECKS
	unsigned char	*se;		// Pointer to stack sentry (end), where guard word is stored
	unsigned char	*st;		// Pointer to stack top for stack free calcs
	unsigned int	lowtide;	// lowest amount of untouched stack
	BOOL			showStats;	// Set to TRUE, if stack check stats should be displayed
#endif
	unsigned char	*sp;		/* Stack pointer for task		*/
	struct _tcb	*next;
	struct _tcb	*prev;
	int param;
	unsigned char	tid;
}TCB;

typedef void (*TaskType)(volatile TCB *);
					/* POS Function prototypes.	*/
IMPORT	void	init_pos();
IMPORT	BOOL	start_pos(BOOL);
IMPORT	TCB		*create(TaskType task,unsigned stack,int param,BOOL showStats);
IMPORT	void	pause();
IMPORT	void	qtask(TCB *tcb);
IMPORT	TCB		*dqtask(void);
IMPORT	void	pos_trap();
					/* POS macros.			*/
#define CLR_INTR() cli()	//local_irq_save()
#define SET_INTR() sei()	//local_irq_restore()

#ifdef STACK_CHECKS
#define STACKGUARD	((unsigned char)0x5a)
#define WATERMARK	((unsigned char)0xbc)

IMPORT BOOL stack_check(TCB *task,BOOL);
IMPORT void chack_all_tcb();
IMPORT	TCB	*currt;			/* Current running task		*/
#endif

#endif
