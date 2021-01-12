/****************************
/ Testcase 1: SEMAPHORES
/	Right implementation: Task 2 should never execute (The second LED should not turn on)
/ 	Wrong implementation: Task 2 is executing (second LED is turning on and statement is printed to usart)
/	Problem: wrong implementation of priorities of IPCP
/****************************


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
#include "kernel/rosa_sem.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"


//Data block for the semaphores
semaphoreHandle sem1;
semaphoreHandle sem2;


//Data blocks for the tasks
#define T1_STACK_SIZE 0x40
static int t1_stack[T1_STACK_SIZE];
static tcb t1_tcb;

#define T2_STACK_SIZE 0x40
static int t2_stack[T2_STACK_SIZE];
static tcb t2_tcb;

/*************************************************************
 * Task1
 * Takes Semaphores
 * turns on LED0
 * installs task 2 which should not execute
 * give sem 2
 * delete task 2
 * give sem 1 
 * switch LED0 off
 ************************************************************/
void task1(void)
{
	while(1) {
		ROSA_semaphoreTake(&sem1,0);
		ROSA_semaphoreTake(&sem2,0);
		ledOn(LED0_GPIO);
		ROSA_tcbInstall(&t2_tcb);
		delay_ms(150);
		ROSA_semaphoreGive(&sem2);
		
		ROSA_tcbDelete(&t2_tcb);
		
		ROSA_semaphoreGive(&sem1);
		ledOff(LED0_GPIO);
	}
}

/*************************************************************
 * Task2
 * Should never run 
 * Toggles LED1 on and write to USART
 ************************************************************/
void task2(void)
{
	while(1) {
		ledOn(LED1_GPIO);
		usartWriteLine(USART,"ENTERED AND RAN TASK 2, THAT SHOULD NOT HAVE HAPPENED IN PROPPER SEM HANDLING!");
	}
}



/*************************************************************
 * Main function for TESTCASE 1
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	// Create needed Semaphores
	sem1=ROSA_semaphoreCreate(8);
	sem2=ROSA_semaphoreCreate(4);
	
	//Create Tasks
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2,6, t2_stack, T2_STACK_SIZE);
	
	
	//Start the ROSA kernel
	ROSA_schedulerStart();
	
	/* Execution will never return here */
	while(1);
}
