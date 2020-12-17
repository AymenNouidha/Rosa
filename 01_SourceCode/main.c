/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// ///''\\,
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

//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_sem.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"



//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;

#define T2_STACK_SIZE 0x40
static int t2_stack[T2_STACK_SIZE];
static tcb t2_tcb;

#define T3_STACK_SIZE 0x40
static int t3_stack[T3_STACK_SIZE];
static tcb t3_tcb;

ticktime prevWakeTime;

static semaphore_handle sem1;
static semaphore_handle sem2;
static semaphore_handle sem3;
static semaphore_handle sem4;

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	while(1) {
		//ROSA_tcbInstall(&t3_tcb);
		//ledOff(LED1_GPIO);
		delay_ms(100);
		ledOn(LED0_GPIO);
		ROSA_tcbDelete(&t1_tcb);
		//delay_ms(500);
		//ledOff(LED0_GPIO);
		//usartWriteLine(USART,"Task1");
		//delay_ms(500);
		//interruptDisable();
		//ROSA_yield();
		//interruptEnable();
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task2(void)
{
	//int a;
	while(1) {
		
		ROSA_semaphoreTake(&sem4,100);
		ledOn(LED1_GPIO);
		ROSA_semaphoreGive(&sem4);
		
		
		//ledOn(LED1_GPIO);
		//ROSA_sysTickWait(1000);
		
		//ROSA_sysTickWait(100);
		//ledOff(LED1_GPIO);
		//ROSA_sysTickWait(1000);
		
		//ROSA_sysTickWait(100);
		//ROSA_yield();
	}
}

/*************************************************************
 * Task3
 * 
 * 
 ************************************************************/
void task3(void)
{
	while(1) {
		ledOn(LED2_GPIO);
		//delay_ms(300);
		
		//ROSA_sysTickWaitUntil(&prevWakeTime,100);
		//ledOff(LED3_GPIO);
		//ROSA_sysTickWait(1000);
		ROSA_semaphoreTake(&sem1,0);
		ROSA_semaphoreTake(&sem2,0);
		ROSA_semaphoreTake(&sem3,0);
		ROSA_semaphoreTake(&sem4,0);
		ledOff(LED2_GPIO);
		ledOff(LED0_GPIO);
		ROSA_sysTickWait(100);
		prevWakeTime=prevWakeTime+100;
		ROSA_semaphoreGive(&sem4);
		ROSA_semaphoreGive(&sem3);
		ROSA_semaphoreGive(&sem2);
		ROSA_semaphoreGive(&sem1);
		//ROSA_sysTickWaitUntil(&prevWakeTime,100);
		//prevWakeTime=prevWakeTime+100;
		ledOff(LED0_GPIO);
		//ROSA_tcbDelete(&t3_tcb);
		//ROSA_yield();
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	prevWakeTime=100;
	//Create tasks and install them into the ROSA kernel
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2,2, t2_stack, T2_STACK_SIZE);
	ROSA_tcbInstall(&t2_tcb);
	
	ROSA_tcbCreate(&t3_tcb, "tsk3", task3,3, t3_stack, T3_STACK_SIZE);
	ROSA_tcbInstall(&t3_tcb);
	
	// Create Semaphores
	sem1=ROSA_semaphoreCreate(7);
	sem2=ROSA_semaphoreCreate(6);
	sem3=ROSA_semaphoreCreate(8);
	sem4=ROSA_semaphoreCreate(5);
	
	//interruptEnable();
	//Start the ROSA kernel
	ROSA_schedulerStart();
	/* Execution will never return here */
	while(1);
}
