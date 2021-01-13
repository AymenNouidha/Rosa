/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'\\,

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

#ifndef _ROSA_TIMER_H_
#define _ROSA_TIMER_H_

#include <avr32/io.h>
#include "stdint.h"
/***********************************************************
 * Typedefs
 ***********************************************************/
// Datatype for the system time tracking
typedef uint32_t ticktime;
#define MAX_TICK_TIME 4294967295U

/***********************************************************
 * Kernel timer functions
 ***********************************************************/
extern void timerInit(unsigned int);
extern void timerReset(void);
extern void timerStart(void);
extern void timerStop(void);

//The timer interrupt service routine
void timerISR(void);
extern void timerClearInterrupt(void);

//Timer period functions
int timerPeriodSet(unsigned int ms);
extern void timerPrescaleSet(int);
extern void timerRCSet(int);

//Timer period variables
extern int timerPrescale;
extern int timerRC;


// User API functions
ticktime ROSA_sysGetTickCount();

//Absolute delay
void ROSA_sysTickWaitUntil(ticktime *previousWakeTime, ticktime timeIncrement);

//Relative Delay
void ROSA_sysTickWait(ticktime ticksToWait);

#endif /* _ROSA_TIMER_H_ */