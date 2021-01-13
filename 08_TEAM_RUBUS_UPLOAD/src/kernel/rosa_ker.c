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
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"

#include "kernel/rosa_utils.h"
#include "kernel/rosa_sem.h"
//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"
//#include "drivers/delay.h"
/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel and are ready to execute
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
 * WAITINGLIST
 *
 * Comment:
 * 	Global variables that contain the list of waiting/ delayed tasks
 **********************************************************/
tcb * WAITINGLIST;

/***********************************************************
* taskNumber
*
* Comment:
* Tracks the number of Tasks created and deleted
**********************************************************/
static unsigned int taskNumber;

/***********************************************************
* rosaInit
*
* Comment:
* Variable for checking whether the system in initialized before it is started
**********************************************************/
unsigned int rosaInit = 0;


//Data blocks for the idle task
#define IDLE_STACK_SIZE 0x40
static int IDLE_stack[IDLE_STACK_SIZE];
static tcb IDLE_tcb;

/*************************************************************
 * IDLE task function
 ************************************************************/
void IDLE(void)
{
	while(1) {
	}
}


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

	//Initialize the timer and the interrupt to 1 ms period.
	
	interruptInit();
	timerInit(1);
	timerPeriodSet(1);
	
	rosaInit = 1;
	//Install IDLE Task
	ROSA_tcbCreate(&IDLE_tcb, "IDLE", IDLE,0, IDLE_stack, IDLE_STACK_SIZE);
	tcb * tmptcb;
	tmptcb=&IDLE_tcb;
	tmptcb->priority = 0;
	ROSA_tcbInstall(&IDLE_tcb);
	
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * Create the TCB with correct values.
 * Return : unsigned int
 *         0 - Unsuccessful
 *		   1 - Successful
 *         2 - Out of Memory
 *         3 - Task Handle is busy
 **********************************************************/
unsigned int ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, unsigned int taskPrio, int * tcbStack, int tcbStackSize)
{
	// Handle if rosa is not initialized
	if(!rosaInit) return 0;                //unsuccessful
	// Checks if TCB is already installed
	// TCB can be recreated several times but its only when
	// its installed then it is linked to the list
	if (TCBLIST != NULL)
	{
		// Instead of iterating the whole list and comparing, we can
		// compare the state. NOTE: the state values the macros are given
		// values see rosa_def.h

		if (tcbTask->state == RUN || tcbTask->state == READY || tcbTask->state == DELAY)
		{
			return 3;
		}
	}
	// Checks if TCB is greater than MAXTASKNUMBER
	if (taskNumber > MAXTASKNUMBER) {
		return 2;
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
	
	tcbTask->lastsem = NULL;
	
	// Set the task priority and increment by one to ensure the idle task has the lowest prio
	tcbTask->priority = taskPrio+1;
	
	// Set task state to NULL
	tcbTask->state = NULL;

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
	interruptEnable();
	return 1;
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
	// Handle if rosa is not initialized
	if(!rosaInit) return;                //unsuccessful
	
	if (tcbTask->state == RUN || tcbTask->state == READY || tcbTask->state == DELAY || tcbTask->state == DELETED)
	{
		return;
	}
	else
	{
		// increment task number
		taskNumber++;
		interruptDisable();
		// Handle if tcbtask has higher priority to EXECTASK
		// Preemption should happen
		if(tcbTask->priority > EXECTASK->priority && systemStarted())
		{
			
			//insert tcbtask
			ROSA_prv_insertTaskToTCBLIST(tcbTask);
			//if higher priority yield
			ROSA_yield();	
		}
		else
		{
			ROSA_prv_insertTaskToTCBLIST(tcbTask);	
		}
		
		interruptEnable();
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

unsigned int ROSA_tcbDelete(tcb* tcbTask)
{

	unsigned statusVal = 0;
	semaphoreHandle* nextsemaphore = tcbTask->lastsem;
	//Give all taken semaphores back
	while(nextsemaphore != NULL)
	{
		nextsemaphore->isFree = true;
		nextsemaphore->storedPrio = 0;
		nextsemaphore = nextsemaphore->nextsem;
	}
	if(EXECTASK==tcbTask){
		//Startup exception
		if(EXECTASK == TCBLIST)
		{
			tcb * tmpTcb=TCBLIST;
			ROSA_prv_extractTaskFromLIST(tmpTcb);
			tmpTcb->state=RUN;

		}
		// Set task state to deleted
		tcbTask->state = DELETED;
		//decrease the task number
		taskNumber--;
		// call dispatch to schedule next task
		dispatch();
		// restore the task context
		Rosa_contextRestore();
		return 1;
	}
	taskNumber--;
	statusVal = ROSA_prv_extractTaskFromLIST(tcbTask);
	return statusVal;
	
}