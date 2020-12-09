/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

//Kernel includes
#include <stdlib.h>
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"
/****************************************
   PRIVATE PROTOTYPES
******************************************/
void _remove(tcb *tcbTask);

/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
tcb * TCBLIST;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
tcb * EXECTASK;

/***********************************************************
* taskNumber
*
* Comment:
* Tracks the number of Tasks created and deleted
**********************************************************/
static unsigned int taskNumber;

/***********************************************************
* sysStarted
*
* Comment:
* Toggles when the system starts
**********************************************************/
//static unsigned int sysStarted = 0;

/***********************************************************
 * ROSA_init
 *
 * Comment:
 * 	Initialize the ROSA system
 *
 **********************************************************/
void ROSA_init(void)
{
	//Do initialization of I/O drivers
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST = NULL;
	EXECTASK = NULL;

	//Initialize the timer to 10 ms period.
	timerInit(10);
	//Start the timer
	timerStart();
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create the TCB with correct values.
 * Return : unsigned int
 *         0 - Unsuccessful
 *		   1 - Successful
 *         2 - Out of Memory
 *         3 - Task Handle is busy
 **********************************************************/
unsigned int ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, unsigned int taskPrio, int * tcbStack, int tcbStackSize)
{
	
	unsigned int statusVal = 0;
	
	// Checks if TCB is already installed
	// TCB can be recreated several times but its only when
	// its installed then it is linked to the list
	if(TCBLIST != NULL)
	{
		// Instead of iterating the whole list and comparing, we can 
		// compare the state. NOTE: the state values the macros are given 
		// random values see rosa_def.h 
		
		if(tcbTask->state == RUN || tcbTask->state == READY || tcbTask->state == DELAY)
		{
				statusVal = 3;
				return statusVal;
		}
	}
	
	// Checks if TCB is greater than MAXTASKNUMBER
	if(taskNumber > MAXTASKNUMBER){
		statusVal = 2;
		return statusVal;
	}
	
	interruptDisable();
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;
	
	// Set the task priority
	tcbTask->priority = taskPrio;
	

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;


	//Set the initial SR.
	tcbTask->savesr = ROSA_INITIALSR;

	//Initialize context.
	contextInit(tcbTask);
	taskNumber++;
	statusVal = 1;
	interruptEnable();
	return statusVal;
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(tcb * tcbTask)
{
	//Checks if TCB is already installed
	if(tcbTask->state == RUN || tcbTask->state == READY || tcbTask->state == DELAY)
	{
		return;
	}
	else
	{
		interruptDisable();
		tcb * tcbTmp;

		/* Is this the first tcb installed? */
		if(TCBLIST == NULL)
		{
			//Install the first tcb
			tcbTask->prevtcb = NULL;
			tcbTask->nexttcb = NULL;
			TCBLIST = tcbTask;			
		}
		else 
		{
			tcbTmp = TCBLIST;
			// Check current highest priority is compared to task priority
			if(tcbTmp->priority < tcbTask->priority)
			{
				// If its smaller then assign task as current head
				// new task prev to NULL
				tcbTask->prevtcb = NULL;
				tcbTask->nexttcb = tcbTmp;
				// Replace TCBLIST previous with new tcbTask
				tcbTmp->prevtcb = tcbTask;
				// Move TCBLIST to the head
				TCBLIST = tcbTask;
				tcbTask->state = READY;
				/* Checks for systemstarted(), if true, force a reschedule */
				//if(systemstarted()){
					// schedule();
					//}
				interruptEnable();
				return;
			}
			
			// Iterate until tcbTmp next is null and 
			//Jump out of loop if tcbTmp not greater and equal to tcbTask priority NOT RUNNING PRIORITY 				
			while(tcbTmp->nexttcb != NULL && tcbTmp->priority >= tcbTask->priority) 
			{
				// Update tcbTmp
				tcbTmp = tcbTmp->nexttcb;
			}
			//Handle when task priority is greater
			if(tcbTmp->priority < tcbTask->priority && tcbTmp != NULL)
			{
				// Insert tcbTask before tcbTmp 
				tcbTask->prevtcb = tcbTmp->prevtcb;
				// Assign nexttcb of tcbTask to tcbTmp
				tcbTask->nexttcb = tcbTmp;
				// Make prev of tcbTmp to tcbTask
				tcbTmp->prevtcb->nexttcb = tcbTask;
				tcbTmp->prevtcb = tcbTask;
				
			}
			// Handle when tcbTmp is last in the list
			if(tcbTmp->nexttcb == NULL)
			{
				// Make tcbTask last on the list
				tcbTask->prevtcb = tcbTmp;			
				tcbTask->nexttcb = NULL;
				tcbTmp->nexttcb = tcbTask;	
			}
		
			tcbTask->state = READY;
			interruptEnable();
			return;
		}
	}
	
}

/***********************************************************
 * ROSA_tcbDelete
 *
 * Comment:
 * 	Removes a task from kernel
 * Return : unsigned int
 *         0 - Unsuccessful
 *		   1 - Successful
 **********************************************************/

unsigned int ROSA_tcbDelete(tcb *tcbTask)
{

	interruptDisable();
	unsigned statusVal = 0;
	
	// Case 1: Handle if task is the executing task
	if(tcbTask == EXECTASK)
	{
		_remove(tcbTask);
		statusVal = 1;
		interruptEnable();
		return statusVal;
	}
	tcb *tcbTmp = TCBLIST;
	// Case 2: Remove task from TCBLIST
	// Traverse the list to search for tcbTask to delete
	while(tcbTmp != NULL)
	{
		if(tcbTmp == tcbTask)
		{
			 _remove(tcbTmp);
			 statusVal = 1;
			 break;
		}
		tcbTmp = tcbTmp->nexttcb;
	}
	
	
	/* Case 3: Remove Task from IDLELIST
	tcbTmp = IDLELIST;
	while(tcbTmp != NULL)
	{
		if(tcbTmp == tcbTask)
		{
			_remove(tcbTmp);
			break;
		}
		tcbTmp = tcbTmp->nexttcb;
	}
	*/
	interruptEnable();
	return statusVal;
}

void _remove(tcb *tcbTask)
{
	//Check if its the first task on queue
	if(tcbTask->prevtcb == NULL)
	{
		// Update TCBLIST to the next on list
		TCBLIST = tcbTask->nexttcb;
	}
	else
	{
		tcbTask->prevtcb->nexttcb = tcbTask->nexttcb;
	}
	// If there are more tasks on the list adjust tcbTask prev
	if(tcbTask->nexttcb != NULL)
	{
		tcbTask->nexttcb->prevtcb = tcbTask->prevtcb;
	}
	
	tcbTask->prevtcb = NULL;
	tcbTask->nexttcb = NULL;
	//free(tcbTask);
	tcbTask = NULL;
	taskNumber--;
	
}