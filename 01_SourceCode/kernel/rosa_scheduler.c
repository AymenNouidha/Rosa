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

static bool _started = false;

bool systemStarted(){
	return _started;
}
/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
/*void scheduler(void)
{
	//Find the next task to execute
	EXECTASK = EXECTASK->nexttcb;
}*/

/***********************************************************
 * dispatch
 *
 * Comment:
 *	Assigns the CPU to the first ready task
 **********************************************************/
void dispatch(void){
	tcb* temp;
	temp = TCBLIST;
	
	if(ROSA_prv_extractTaskFromLIST(temp)){
		temp->state = RUN;
		temp->nexttcb = NULL;
		EXECTASK = temp;
		//interruptEnable();
  }	
}
/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void scheduler(void)
{
	interruptDisable();
	if(EXECTASK->state == DELAY){
		dispatch();
	}
	else if(TCBLIST!=NULL && TCBLIST->priority > EXECTASK->priority){
		//EXECTASK->state = READY;
    ROSA_prv_insertTaskToTCBLIST(EXECTASK);
		dispatch();
	}
	interruptEnable();
	
}

void ROSA_schedulerStart(void)
{
	_started = true;
	timerStart();
	ROSA_start();
	
}