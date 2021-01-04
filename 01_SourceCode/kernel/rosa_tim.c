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

#include "rosa_config.h"
#include "kernel/rosa_tim.h"
#include "drivers/delay.h"
#include "kernel/rosa_int.h"
#include "drivers/led.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_utils.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_scheduler.h"



//Tickcount Variable
ticktime tickCount=0;


/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 * 	Checks for valid interrupt and does the tracking of the system time
 * 	Wakes up the Tasks from the WAITINGLIST 
 * 	Does the preemptive scheduling
 **********************************************************/

__attribute__((__interrupt__))
void timerISR(void)
{
	int sr, count;
	tcb* temp;
	volatile avr32_tc_t * tc = &AVR32_TC;
	
	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if(sr & AVR32_TC_CPCS_MASK){
		count = 0;
		
		//Increment the System Time
		tickCount++;
		
		//Check the System Time for overflow
		if(tickCount>=MAX_TICK_TIME)
		{
			tickCount=tickCount-MAX_TICK_TIME;
		}
		
		//Check the Waitinglist and wake up the tasks
		if(WAITINGLIST!=NULL){
			temp = WAITINGLIST;
			//Wake up tasks which needs to be woken up
			while(temp->waketime <= tickCount && temp != NULL){
				
				if(ROSA_prv_extractTaskFromLIST(temp)){
					ROSA_prv_insertTaskToTCBLIST(temp);
					temp = WAITINGLIST;
				}
				//Increment counter for woken up tasks
				count++;	
			}
			
		
		}
		
		//when at least one task woke up, do rescheduling
		if(count > 0)
		{
			 ROSA_yieldFromISR();
			 //contextSaveFromISR();
			 //scheduler();
			 //contextRestoreFromISR();
		}
	}
	
}
			
	
		


/***********************************************************
 * timerPeriodSet
 *
 * Comment:
 * 	Set the timer period to 'ms' milliseconds.
 *
 **********************************************************/
int timerPeriodSet(unsigned int ms)
{

	int rc, prescale;
	int f[] = { 2, 8, 32, 128 };
	//FOSC0 / factor_prescale * time[s];
	prescale = AVR32_TC_CMR0_TCCLKS_TIMER_CLOCK5;
	rc = FOSC0 / f[prescale - 1] * ms / 1000;
	timerPrescaleSet(prescale);
	timerRCSet(rc);
	return rc * prescale / FOSC0;
}

/********************************************************************************************
 * ROSA_getTickCount
 *
 * Comment:
 * 	returns the value of the tick count which keeps track of the system schedulertick count.
 * Return : ticktime (uint32_t)
 *        - Actual value of the system scheduler tick count
 ***********************************************************************************************/

ticktime ROSA_sysGetTickCount()
{
	//return the actual scheduler tick time
	return tickCount;
}



/********************************************************************************************
 * ROSA_sysTickWaitUntil
 *
 * Comment:
 * 	Suspends the using Task until a certain point in time is reached (previousWake-Time+timeIncrement).
 *	Puts the calling task (EXECTASK) in the Delaylist and write the wake up time to the TCB and reschedule
 *	
 ***********************************************************************************************/
void ROSA_sysTickWaitUntil(ticktime *previousWakeTime, ticktime timeIncrement)
{
	
	tcb * tmpTcb;
	tmpTcb=EXECTASK;
	ticktime wakeUpTime;
	
	//calculate the wakeUpTime
	wakeUpTime=*previousWakeTime+timeIncrement;
	
	//check possible wakeUpTime for overflow
	if(wakeUpTime>=MAX_TICK_TIME)
	{
		ticktime stor;
		stor=MAX_TICK_TIME-*previousWakeTime;
		wakeUpTime=timeIncrement-stor;
	}
	
	//Set the Wake up time in the TCB
	EXECTASK->waketime=*previousWakeTime+timeIncrement;
	
	//Insert the Task to the Waitinglist
	ROSA_prv_insertTaskToWAITINGLIST(tmpTcb);

	//Yield the system (context save, reschedule, context restore)
	ROSA_yield();

	
	
	
}

/**************************************************************************
 * ROSA_sysTickWait
 *
 * Comment:
 * 	Suspends the running Task for the given amount of system scheduler ticks. 
 *  If the delay is 0, it forces a rescheduling.
 *
 ****************************************************************************/
void ROSA_sysTickWait(ticktime ticksToWait)
{
	
	
	ticktime startTime;
	//get the actual system Time as the starttime of the delay
	startTime=tickCount;
	
	//Call absolute delay Function with the calculated parameters 
	ROSA_sysTickWaitUntil(&startTime,ticksToWait);
	
	
}