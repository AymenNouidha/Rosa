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

int x=1;
/***********************************************************
 * timerInterruptHandler
 *
 * Comment:
 * 	This is the basic timer interrupt service routine.
 **********************************************************/

__attribute__((__interrupt__))
void timerISR(void)
{
	int sr, count;
	tcb* temp;
	volatile avr32_tc_t * tc = &AVR32_TC;
	

	//ledOn(LED2_GPIO);
	//Read the timer status register to determine if this is a valid interrupt
	sr = tc->channel[0].sr;
	if(sr & AVR32_TC_CPCS_MASK){
		count = 0;
		//contextSaveFromISR();
		tickCount++;
		if(tickCount>=MAX_TICK_TIME)
		{
			tickCount=tickCount-MAX_TICK_TIME;
		}
		//Rosa_contextSave();
		if(WAITINGLIST!=NULL){
			temp = WAITINGLIST;
			while(temp->waketime <= tickCount && temp != NULL){
				
				if(ROSA_prv_extractTaskFromLIST(temp)){
					ROSA_prv_insertTaskToTCBLIST(temp);
					temp = WAITINGLIST;
				}
				count++;	
			}
			
		
		}
		if(count > 0)
		{
			//contextRestoreFromISR();
			 contextSaveFromISR();
			 scheduler();
			 //dispatch();
			 contextRestoreFromISR();
			 //Rosa_contextRestore();
		}
		//contextRestoreFromISR();
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
 * Return : uint32_t
 *        - Actual value of the system scheduler tick count
 ***********************************************************************************************/

ticktime ROSA_sysGetTickCount()
{
	return tickCount;
}



/********************************************************************************************
 * ROSA_sysTickWaitUntil
 *
 * Comment:
 * 	Suspends the using Task until a certain point in time is reached (previousWake-Time+timeIncrement).
 *
 ***********************************************************************************************/
void ROSA_sysTickWaitUntil(ticktime *previousWakeTime, ticktime timeIncrement)
{
	
	// Assumption: check time for Overflow and beeing valid
	tcb * tmpTcb;
	tmpTcb=EXECTASK;
	ticktime wakeUpTime;
	//interruptDisable();
	wakeUpTime=*previousWakeTime+timeIncrement;
	if(wakeUpTime>=MAX_TICK_TIME)
	{
		ticktime stor;
		stor=MAX_TICK_TIME-*previousWakeTime;
		wakeUpTime=timeIncrement-stor;
	}
	
	EXECTASK->waketime=*previousWakeTime+timeIncrement;
	ROSA_prv_insertTaskToWAITINGLIST(tmpTcb);
	//interruptEnable();
	ROSA_yield();
// 	Rosa_contextSave();
// 	scheduler();
// 	Rosa_contextRestore();
	
	//interruptEnable();
	//dispatch();
	
	
	
}

/**************************************************************************
 * ROSA_sysTickWait
 *
 * Comment:
 * 	Suspends the using Task for the given amount of system scheduler ticks. 
 *  If the delay is 0, it forces a rescheduling.
 *
 ****************************************************************************/
void ROSA_sysTickWait(ticktime ticksToWait)
{
	//interruptDisable();
	ticktime startTime;
	startTime=tickCount;
	ROSA_sysTickWaitUntil(&startTime,ticksToWait);
	
	//interruptEnable();
}