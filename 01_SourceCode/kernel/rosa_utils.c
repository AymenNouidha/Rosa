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

//Maybe unneccessary, because of the state variable in the tcb
/***********************************************************
 * ROSA_prv_isTaskinLIST
 *
 * Comment:
 * 	searches for a task in a specific list
 * 	
 * RETURNVALS:
 *	1 Task is in specified List
 *	0 Task was not found in the List
 **********************************************************/
/*ROSA_prv_isTaskinLIST(tcb * tcbTask, tcb* LIST)
{
	tcb * tmpTcb;
	
	tmpTcb=LIST;
	
	while(tmpTcb!=tcbTask)
	{
		tmpTcb=tmpTcb->nexttcb;
		if(tmpTcb==NULL)
		{
			return 0; // TASK IS NOT IN THE LIST
		}
	}
	
	return 1; //TASK IS IN THE LIST
}*/


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
	if(tcbTask==NULL)
	{
		interruptEnable();
		return 1;
	}
	// Startup exception
	if(EXECTASK == TCBLIST)
	{
		tcb * tmpTcb=TCBLIST;
		ROSA_prv_extractTaskFromLIST(tmpTcb);
		tmpTcb->state=RUN;

		
	}
	if(WAITINGLIST==NULL)
	{
		tcbTask->nexttcb = NULL;
		WAITINGLIST = tcbTask;
		tcbTask->state = DELAY;
		interruptEnable();
		return 1;
	}
	if(tcbTask->state != DELAY)
	{
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
	if(tcbTask==NULL)
	{
		interruptEnable();
		return 1;
	}
	if(TCBLIST == NULL)
	{
		tcbTask->nexttcb = NULL;
		tcbTask->state = READY;
		TCBLIST = tcbTask;
		interruptEnable();
		return 1;
	}
	// Startup exception
	if(EXECTASK == TCBLIST)
	{
		tcb * tmpTcb=TCBLIST;
		ROSA_prv_extractTaskFromLIST(tmpTcb);
		tmpTcb->state=RUN;
	}
	if(tcbTask->state != READY)
	{
		
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
 * 	extracts a Task to the specified LIST
 * 	
 * RETURNVALS:
 *	1 Successfull
 *	0 ERROR OCCURED
 **********************************************************/
int ROSA_prv_extractTaskFromLIST(tcb * tcbTask)
{
	interruptDisable();
	if(tcbTask->state != DELETED)
	{
		if(tcbTask->state == READY)
		{
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
		else if(tcbTask->state == DELAY)
		{
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
	
	
	
	
	















