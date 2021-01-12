/****************************
/ Testcase 2: SEMAPHORES AND DELAY IN SEM TAKE
/	Right Implementation: LED0 goes on, Led1 goes on, Led2 goes on, led0 goes off, led1 goes of and then starts blinking  
/	Wrong Implementation: LED2 will never blink 
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

/*************************************************************
 * Task1
 * 
 * 
 ************************************************************/
void task1(void)
{
	while(1) {
		ledOn(LED0_GPIO);
		ROSA_semaphoreTake(&sem1,0);
		ROSA_tcbInstall(&t2_tcb);
		ROSA_tcbInstall(&t3_tcb);
		delay_ms(150);
		ROSA_sysTickWait(3000);
		ROSA_semaphoreGive(&sem1);
		ledOff(LED0_GPIO);
	}
}

/*************************************************************
 * Task2
 ************************************************************/
void task2(void)
{
	while(1) {
		ledOn(LED1_GPIO);
		ROSA_semaphoreTake(&sem1,3000);
		delay_ms(250);
		ROSA_semaphoreGive(&sem1);
		ledOff(LED1_GPIO);
	}
}

/*************************************************************
 * Task3
 ************************************************************/
void task3(void)
{
	while(1) {
		ledOn(LED2_GPIO);
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
	sem1=ROSA_semaphoreCreate(6);
	
	//Create Tasks
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	
	ROSA_tcbCreate(&t2_tcb, "tsk2", task2,6, t2_stack, T2_STACK_SIZE);
	ROSA_tcbCreate(&t3_tcb, "tsk3", task3,4, t3_stack, T3_STACK_SIZE);
	
	
	//Start the ROSA kernel
	ROSA_schedulerStart();
	
	/* Execution will never return here */
	while(1);
}
