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
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

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
unsigned int taskNumber;

/***********************************************************
* taskNumber
*
* Comment:
* Toggles when the system starts
**********************************************************/
static unsigned int sysStarted = 0;

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

	//Initialize the timer to 100 ms period.
	//...
	//timerInit(100);
	//...
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
	interruptDisable();
	// Checks if TCB is already created
	if(TCBLIST != NULL){
		tcb * tcbTmp;
		tcbTmp = TCBLIST;
		while(tcbTmp != NULL){
			if(tcbTmp == tcbTask){
				statusVal = 3;
				return statusVal;
			}
			tcbTmp = tcbTmp->nexttcb;
		}
	}
	
	// Checks if TCB is greater than MAXTASKNUMBER
	if(taskNumber > MAXTASKNUMBER){
		statusVal = 2;
		return statusVal;
	}
	
	
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;
	//tcbTask->priority = taskPrio;

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
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		TCBLIST->nexttcb = NULL;			
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != NULL) {
			if(tcbTmp == tcbTask)
				return;
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		tcbTask->nexttcb = NULL;			
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
	unsigned statusVal = 0;
	return statusVal;
}