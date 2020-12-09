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

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"

//Include configuration
#include "rosa_config.h"

//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
static int ti_stack[T1_STACK_SIZE];
static tcb ti_tcb;

static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;


static int t2_stack[T1_STACK_SIZE];
static tcb t2_tcb;


static int t3_stack[T1_STACK_SIZE];
static tcb t3_tcb;

static int t4_stack[T1_STACK_SIZE];
static tcb t4_tcb;

static int t5_stack[T1_STACK_SIZE];
static tcb t5_tcb;

static int t6_stack[T1_STACK_SIZE];
static tcb t6_tcb;

static int t7_stack[T1_STACK_SIZE];
static tcb t7_tcb;

/*************************************************************
 * TaskIDLE
 * Just sorta sits there
 * idunno ¯\_(ツ)_/¯
 ************************************************************/
 void taskIDLE(void)
 {
 	while(1) {
 		delay_ms(1000);
		//Maybe some sort of print command can be added here
		//to show all tasks in the TCBLIST? If it prints out
		//nothing we would know everything was done right.
 	}
 }

/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/


/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
 void task4()
 {
	while(1){
		ledOn(LED3_GPIO);
		ledOff(LED2_GPIO);
		delay_ms(150);
		ROSA_yield();
	}
	
}
void task3()
{
	unsigned char num = 0;
	while(1){
		ledOn(LED2_GPIO);
		ledOff(LED1_GPIO);
		delay_ms(150);
		
		ROSA_tcbCreate(&t4_tcb, "tsk4", task4,0, t4_stack, T1_STACK_SIZE);
 		ROSA_tcbInstall(&t4_tcb);
		num++;
		if(num == 11)
		{
			ROSA_tcbDelete(&t4_tcb);
			ledOff(LED3_GPIO);	
		}
		ROSA_yield();
		
	}
}

void task2(void)
{
	unsigned char num = 0;
	while(1) {
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		delay_ms(150);

		ROSA_yield();
		num++;
		if(num == 3)
		{
			ROSA_tcbDelete(&t7_tcb);
			ledOff(LED6_GPIO);
		}
		if(num == 13)
		{
			ROSA_tcbDelete(&t3_tcb);
			ledOff(LED2_GPIO);
		}
		if(num == 20)
		{
			ROSA_tcbDelete(&t2_tcb);
		}
	}
}
void task7(void){
	
	while(1)
	{
		ledOff(LED5_GPIO);
		ledOn(LED6_GPIO);
		delay_ms(150);
		ROSA_yield();
	}
}

void task6(void){
	while(1)
	{
		ledOff(LED4_GPIO);
		ledOn(LED5_GPIO);
		delay_ms(150);
		ROSA_tcbCreate(&t3_tcb, "tsk3", task3,1, t3_stack, T1_STACK_SIZE);
		ROSA_tcbInstall(&t3_tcb);
		ROSA_tcbCreate(&t7_tcb, "tsk7", task7,5, t7_stack, T1_STACK_SIZE);
		ROSA_tcbInstall(&t7_tcb);
		ROSA_yield();
	}
}

void task5(void)
{
	unsigned char num = 0;
	while (1)
	{
		ledOff(LED3_GPIO);
		ledOn(LED4_GPIO);
		delay_ms(150);
		ROSA_tcbCreate(&t6_tcb, "tsk6", task6,4, t6_stack, T1_STACK_SIZE);
		ROSA_tcbInstall(&t6_tcb);
		
		num++;
		if(num == 5)
		{
			ROSA_tcbDelete(&t6_tcb);
			ledOff(LED5_GPIO);
		}
		if(num == 7)
		{
			ROSA_tcbDelete(&t5_tcb);
			ledOff(LED4_GPIO);
		}
		ROSA_yield();
	}
}

 void task1(void)
 {
	 unsigned char num = 0;
	 while(1) {
		 ledOn(LED0_GPIO);
		 if(num > 13)
			ledOff(LED1_GPIO);
		 else if(num == 25)
		 {
			 ledOff(LED0_GPIO);
			 ROSA_tcbDelete(&t1_tcb);
		 }
		 else 
			ledOff(LED6_GPIO);
		 delay_ms(150);
		 ROSA_yield();
		 num++;
		 
			
	 }
 }

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();

 	//Create tasks and install them into the ROSA kernel
<<<<<<< HEAD
 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,2, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2,2, t2_stack, T1_STACK_SIZE);
=======
	ROSA_tcbCreate(&ti_tcb, "tskIDL", taskIDLE, 0, ti_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&ti_tcb);
 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, 1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, 2, t2_stack, T1_STACK_SIZE);
>>>>>>> Andreas
	ROSA_tcbInstall(&t2_tcb);
	ROSA_tcbCreate(&t5_tcb, "tsk5", task5,3, t5_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t5_tcb);

	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
