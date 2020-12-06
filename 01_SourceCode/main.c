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
#include "kernel/rosa_ker.h"
#include "kernel/rosa_utils.h"

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

#define T4_STACK_SIZE 0x40
static int t4_stack[T4_STACK_SIZE];
static tcb t4_tcb;
/*************************************************************
 * Task1
 * LED0 lights up
 * LED1 goes dark
 ************************************************************/
void task1(void)
{
	while(1) {
		ticktime a;
		a=ROSA_sysGetTickCount();
		//ROSA_prv_insertTaskToDELAYLIST(&t4_tcb);
		if(a%2){
			ledOn(LED0_GPIO);
			ROSA_prv_extractTaskFromDELAYLIST(&t2_tcb);
			if(!ROSA_prv_isTaskinDELAYLIST(&t2_tcb)){
				ledOff(LED1_GPIO);
			}
		}
		else
		{
			ledOff(LED0_GPIO);
			ROSA_prv_insertTaskToDELAYLIST(&t2_tcb);
			if(ROSA_prv_isTaskinDELAYLIST(&t2_tcb)){
				ledOn(LED1_GPIO);
			}
		}
		if(ROSA_prv_isTaskinDELAYLIST(&t3_tcb)){
			ledOn(LED2_GPIO);
		}
		else{
			ledOff(LED2_GPIO);
		}
		if(ROSA_prv_isTaskinDELAYLIST(&t4_tcb)){
			ledOn(LED3_GPIO);
		}
		else{
			ledOff(LED3_GPIO);
		}
		if((a>>4)%2){
			ROSA_prv_extractTaskFromDELAYLIST(&t3_tcb);
		}
		if((a>>5)%2){
			ROSA_prv_insertTaskToDELAYLIST(&t3_tcb);
			ROSA_prv_extractTaskFromDELAYLIST(&t4_tcb);
		}
		if((a>>6)%2){
			ROSA_prv_insertTaskToDELAYLIST(&t4_tcb);
		}
		/*if((a>>1)%2){
			ledOn(LED1_GPIO);
		}
		else
		{
			ledOff(LED1_GPIO);
		}
		if((a>>2)%2){
			ledOn(LED2_GPIO);
		}
		else
		{
			ledOff(LED2_GPIO);
		}
		if((a>>3)%2){
			ledOn(LED3_GPIO);
		}
		else
		{
			ledOff(LED3_GPIO);
		}
		if((a>>4)%2){
			ledOn(LED4_GPIO);
		}
		else
		{
			ledOff(LED4_GPIO);
		}
		if((a>>5)%2){
			ledOn(LED5_GPIO);
		}
		else
		{
			ledOff(LED5_GPIO);
		}
		if((a>>6)%2){
			ledOn(LED6_GPIO);
		}
		else
		{
			ledOff(LED6_GPIO);
		}
		if((a>>7)%2){
			ledOn(LED7_GPIO);
		}
		else
		{
			ledOff(LED7_GPIO);
		}*/
		
		ROSA_priv_incrementTickCount(1);
		/*ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);*/
		delay_ms(1000);
		
		
		ROSA_yield();
	}
}

/*************************************************************
 * Task2
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task2(void)
{
	while(1) {
		ledOff(LED0_GPIO);
		ledOn(LED1_GPIO);
		delay_ms(150);
		ROSA_yield();
	}
}

/*************************************************************
 * Task3
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task3(void)
{
	while(1) {
		ledOff(LED3_GPIO);
		ledOn(LED4_GPIO);
		delay_ms(150);
		ROSA_yield();
	}
}

/*************************************************************
 * Task4
 * LED0 goes dark
 * LED1 lights up
 ************************************************************/
void task4(void)
{
	while(1) {
		ledOff(LED4_GPIO);
		ledOn(LED3_GPIO);
		delay_ms(150);
		ROSA_yield();
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
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2, t2_stack, T2_STACK_SIZE);
	ROSA_prv_insertTaskToDELAYLIST(&t2_tcb);
	//ROSA_tcbInstall(&t2_tcb);
	ROSA_tcbCreate(&t3_tcb, "tsk3", task3, t3_stack, T3_STACK_SIZE);
	ROSA_prv_insertTaskToDELAYLIST(&t3_tcb);
	//ROSA_tcbInstall(&t3_tcb);
	ROSA_tcbCreate(&t4_tcb, "tsk4", task4, t4_stack, T4_STACK_SIZE);
	ROSA_prv_insertTaskToDELAYLIST(&t4_tcb);
	//ROSA_tcbInstall(&t4_tcb);
	
	
	//Start the ROSA kernel
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
