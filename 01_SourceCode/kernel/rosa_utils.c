/*
 * rosa_utils.c
 * This library is for the purpose of LIST HANDLING in the ROSA Kernel
 */ 

#include "kernel/rosa_utils.h"

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_int.h"


/***********************************************************
 * ROSA_prv_insertTaskToWAITINGLIST
 *
 * Comment:
 * 	inserts a Task to the WAITINGLIST
 * 	
 * RETURNVALS:
 *	1 Successfull
 *	0 ERROR OCCURED (Task is already in List)
 **********************************************************/
int ROSA_prv_insertTaskToWAITINGLIST(tcb * tcbTask)
{
	interruptDisable();
	// If the task to install is NULL then just return
	if(tcbTask==NULL)
	{
		interruptEnable();
		return 1;
	}
	// Startup exception for removing the first executing task which is assigned twice (in EXECTASK AND TCBLIST)
	if(EXECTASK == TCBLIST)
	{
		tcb * tmpTcb=TCBLIST;
		ROSA_prv_extractTaskFromLIST(tmpTcb);
		tmpTcb->state=RUN;

	}
	
	// If the waitinglist is empty install the task as the first element
	if(WAITINGLIST==NULL)
	{
		tcbTask->nexttcb = NULL;
		WAITINGLIST = tcbTask;
		tcbTask->state = DELAY;
		interruptEnable();
		return 1;
	}
	
	// When the task is not in the delay list sort the task in there
	if(tcbTask->state != DELAY)
	{ 
		// Sort the task regarding the wake up times
		// If the task needs to be the first element in the list insert and return
		if(tcbTask->waketime < WAITINGLIST->waketime)
		{
			tcbTask->nexttcb = WAITINGLIST;
			WAITINGLIST = tcbTask;
			tcbTask->state = DELAY;
			interruptEnable();
			return 1;
		}
		
		tcb * tmpTcb;
		tcb * lstTmpTcb;

		tmpTcb = WAITINGLIST;
		
		// Iterate through the list an insert task (when same waketimes, insert last)
		while(tmpTcb->waketime <= tcbTask->waketime && tmpTcb != NULL)
		{
			lstTmpTcb = tmpTcb;
			tmpTcb = tmpTcb->nexttcb;
		}
		tcbTask->nexttcb = tmpTcb;
		lstTmpTcb->nexttcb = tcbTask;
		tcbTask->state = DELAY;
		interruptEnable();
		return 1;
	}
	
	
	interruptEnable();
	return 0;

}

/***********************************************************
 * ROSA_prv_insertTaskToTCBLIST
 *
 * Comment:
 * 	inserts a Task to the TCBLIST
 * 	
 * RETURNVALS:
 *	1 Successfull
 *	0 ERROR OCCURED (Task is already in List)
 **********************************************************/
int ROSA_prv_insertTaskToTCBLIST(tcb * tcbTask)
{
	interruptDisable();
	// If the task to install is NULL then just return
	if(tcbTask==NULL)
	{
		interruptEnable();
		return 1;
	}
	// If the readylist is empty install the task as the first element
	if(TCBLIST == NULL)
	{
		tcbTask->nexttcb = NULL;
		tcbTask->state = READY;
		TCBLIST = tcbTask;
		interruptEnable();
		return 1;
	}
	// Startup exception for removing the first executing task which is assigned twice (in EXECTASK AND TCBLIST)
	if(EXECTASK == TCBLIST)
	{
		tcb * tmpTcb=TCBLIST;
		ROSA_prv_extractTaskFromLIST(tmpTcb);
		tmpTcb->state=RUN;
	}
	
	// When the task is not in the ready list sort the task in there
	if(tcbTask->state != READY)
	{
	
		// Sort the task regarding the priorities
		// If the task needs to be the first element in the list insert and return
		if(tcbTask->priority > TCBLIST->priority)
		{
			tcbTask->nexttcb = TCBLIST;
			TCBLIST = tcbTask;
			tcbTask->state = READY;
			interruptEnable();
			return 1;
		}
				
		tcb * tmpTcb;
		

		tmpTcb = TCBLIST;
		tcb * lstTmpTcb;
		
		// Iterate through the list an insert task (when same priorities, insert last)
		while(tcbTask->priority <= tmpTcb->priority && tmpTcb != NULL)
		{
			lstTmpTcb = tmpTcb;
			tmpTcb = tmpTcb->nexttcb;
		}
		tcbTask->nexttcb = tmpTcb;
		lstTmpTcb->nexttcb = tcbTask;
		tcbTask->state = READY;
		interruptEnable();
		return 1;
	}
	interruptEnable();
	return 0;

}


/***********************************************************
 * ROSA_prv_extractTaskFromLIST
 *
 * Comment:
 * 	extracts a Task from the List where it is in
 * 	
 * RETURNVALS:
 *	1 Successfull
 *	0 ERROR OCCURED
 **********************************************************/
int ROSA_prv_extractTaskFromLIST(tcb * tcbTask)
{
	interruptDisable();
	// When the task is in a list the check in which list and delete
	if(tcbTask->state != DELETED)
	{
		// when the task is in the ready list
		if(tcbTask->state == READY)
		{
			// when the task is the first element, extract and set LISTHANDLER to the next task
			if(tcbTask == TCBLIST)
			{
				TCBLIST = TCBLIST->nexttcb;
				tcbTask->state = DELETED;
				tcbTask->nexttcb = NULL;
				interruptEnable();
				return 1;
			}
			
			tcb * tmpTcb;

			tmpTcb = TCBLIST;
			
			//Iterate through the list and remove the task
			while(tmpTcb->nexttcb != tcbTask)
			{
				tmpTcb = tmpTcb->nexttcb;
			}
			
			tmpTcb->nexttcb = tcbTask->nexttcb;
			tcbTask->nexttcb = NULL;
			tcbTask->state = DELETED;
			interruptEnable();
			return 1;
			
			
		}
		//when the task is in the delaylist
		else if(tcbTask->state == DELAY)
		{
			// when the task is the first element, extract and set LISTHANDLER to the next task
			if(tcbTask == WAITINGLIST)
			{
				WAITINGLIST = tcbTask->nexttcb;
				tcbTask->state = DELETED;
				tcbTask->nexttcb = NULL;
				interruptEnable();
				return 1;
			}
			
			tcb * tmpTcb;

			tmpTcb = WAITINGLIST;
			
			//Iterate through the list and remove the task
			while(tmpTcb->nexttcb!=tcbTask)
			{
				tmpTcb = tmpTcb->nexttcb;
			}
			
			tmpTcb->nexttcb = tcbTask->nexttcb;
			tcbTask->nexttcb = NULL;
			tcbTask->state = DELETED;
			interruptEnable();
			return 1;
			
		}
		
	}
	interruptEnable();
	return 0;
	
}
	
	
	
	
	















