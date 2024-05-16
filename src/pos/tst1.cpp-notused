#include "Arduino.h"
#include "pos.h"

#define STACK	(unsigned)95 //110 //176 //240

IMPORT	TCB	*currt;			/* Current running task		*/

extern void 	task(volatile TCB *);

void testStart(int no )
{
	int c;
	//int no = 0;
	BOOL result = FALSE;

	init_pos();
	//Serial.print("Enter no of worker tasks between 1 & "); Serial.print(MAXNOTASKS-1); Serial.print("\r\n");

	//scanf("%d",&no);

	delay(1000);
	for (c=0;c<no;c++){
			Serial.print(F("Creating Task:"));Serial.println(c);
		if(!create(task,STACK,c,TRUE))
			Serial.println(F("Could not create task"));
		else
			Serial.println(F("Task created"));
	}

	Serial.print("Worker Tasks created.\r\n");
	delay(100);

	//Serial.print("Leaving main.\r\n");
			// start and turn this thread into the system task
	/*
	if(!start_pos(TRUE)){
		Serial.println(F("Unable to start POS"));
		while(1);//exit(1);
	}
	c = 0;
	FOREVER {
		Serial.print(F("Leaving System task TID: ")); Serial.println(currt->tid);
		pause();
		Serial.print(F("Back to System task TID: ")); Serial.println(currt->tid);
		//if(c++ > 0) exit(1);
	}
	//Serial.print("Panic - back to main.\r\n");

	 */
}

void task(volatile TCB *tcb)
{
	//volatile char dummy[15];
//	int c=0;
//	int i=0;
	volatile TCB *localTcb = tcb;
	char taskLetter = (char)('A'+tcb->tid - 1);
	char tid = tcb->tid;

	Serial.print(F("Starting task ")); Serial.print(localTcb->tid,HEX);
	Serial.print(F(" ")); Serial.println(taskLetter);
	delay(10);
	FOREVER{
//		for(c=0;c<tcb->tid*4+4;c++) putchar(' ');
		Serial.print(F("Leaving task ")); Serial.print(taskLetter); Serial.print(F(" TID: ")); Serial.println(localTcb->tid,HEX);
		delay(10);
		pause();

//		for(c=0;c<tcb->tid*4+4;c++) putchar(' ');
		Serial.print(F("Back to task ")); Serial.print(taskLetter); Serial.print(F(" TID: ")); Serial.println(tid,HEX);
		/*
		if(i++ >3)
			return;
		else{
			Serial.print(F("Iter:  ")); Serial.print((i)); Serial.print(F("-> (TID: ")); Serial.print(currt->tid); Serial.print(")\r\n");
		}
		*/
		delay(100);
	}
}
