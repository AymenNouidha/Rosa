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

#ifndef rosa_def_H_
#define rosa_def_H_

#include "rosa_config.h"
#include "rosa_tim.h"

#ifndef NULL
#define NULL 0
#endif

#define MAXTASKNUMBER 20  //subject to change

#define DELETED 16
#define RUN 2
#define READY 4
#define DELAY 8

/***********************************************************
 * TCB block
 *
 * Comment:
 * 	This struct contain all the necessary information to
 * 	do a context switch
 *
 **********************************************************/
typedef struct tcb_record_t {
	struct tcb_record_t * nexttcb;
	char id[NAMESIZE];		//The task id/name
	void (*staddr) (void);	//Start address
	int *dataarea;			//The stack data area
	int datasize;			//The stack size
	int *saveusp;			//The current stack position
	int SAVER0;				//Temporary work register
	int SAVER1;				//Temporary work register
	int savesr;				//The current status register
	int retaddr;			//The return address
	int savereg[15];		//The CPU registers
	int priority;           //The fixed priority of the task
	int state;				//The state of the task
	ticktime waketime;		//Wakeuptime for DELAYLIST
	struct semaphore_handle *lastsem; //Pointer to the last semaphore taken by the task
} tcb;

#endif /* rosa_def_H_ */
