/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

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

#include "kernel/rosa_scheduler.h"
#include "kernel/rosa_utils.h"
#include "kernel/rosa_int.h"


// Variable for tracking whether the system has started
static bool _started = false;

bool systemStarted(){
	return _started;
}


/***********************************************************
 * dispatch
 *
 * Comment:
 *	Assigns the CPU to the first ready task
 **********************************************************/
void dispatch(void){
	tcb* temp;
	temp = TCBLIST;
	//extract the highest priority task from the TCBLIST and put it in the Executing state
	if(ROSA_prv_extractTaskFromLIST(temp)){
		temp->state = RUN;
		temp->nexttcb = NULL;
		EXECTASK = temp;
  }	
}
/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for context switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task and compares the priorities.
 **********************************************************/
void scheduler(void)
{
	interruptDisable();
	
	// When the running task delays itself, then dispatch the tasks
	if(EXECTASK->state == DELAY){
		dispatch();
	}
	
	// schedule tasks regarding the priorities, forces a context switch when a higher priority task is ready to execute
	else if(TCBLIST!=NULL && TCBLIST->priority > EXECTASK->priority){
		//Insert the running task to the readylist
		ROSA_prv_insertTaskToTCBLIST(EXECTASK);
		//dispatch the tasks
		dispatch();
	}
	interruptEnable();
	
}

/***********************************************************
 * ROSA_schedulerStart
 *
 * Comment:
 * 	Starts the ROSA scheduler with fixed priority preemptive scheduling algorithm.
 *	Synchronisation among tasks will be implemented with IPCP.
**********************************************************/
void ROSA_schedulerStart(void)
{
	//Check whether the system was initialized propperly before
	if(rosaInit==1)
	{
	//Set system Variable to started
	_started = true;
	//Start the timer for the Interrupts (Preemption)
	timerStart();
	//Start the scheduling
	ROSA_start();
	}
	
	while(1);

}