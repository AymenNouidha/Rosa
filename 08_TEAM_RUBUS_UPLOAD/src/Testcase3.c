/****************************
/ Testcase 3: SCHEDULER INTERRUPT AND DYNAMIC CREATION AND DELETION OF TASKS
/ Right implementation: USART output: EXECUTE TASK 1; EXECUTE TASK 2; EXECUTE TASK 3; DELETE TASK 3; DELETE TASK 2; DELETE TASK 1;
/
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

//NEW LINES ADDED IN HERE
void task1();
void task2();
void task3();

/*************************************************************
 * task1
 * 
 * 
 ************************************************************/
void task1(void)
{
	while(1) {
		
		usartWriteLine(USART,"EXECUTE TASK 1\n");
		ROSA_tcbCreate(&t2_tcb, "tsk2", task2,3, t2_stack, T2_STACK_SIZE);
		ROSA_tcbInstall(&t2_tcb);
		delay_ms(100);
		usartWriteLine(USART,"DELETE TASK 1\n");
		ROSA_tcbDelete(&t1_tcb);
	}
}

/*************************************************************
 * Task2
 ************************************************************/
void task2(void)
{
	while(1) {
		
		usartWriteLine(USART,"EXECUTE TASK 2\n");
		ROSA_tcbCreate(&t3_tcb, "tsk3", task3,6, t3_stack, T3_STACK_SIZE);
		ROSA_tcbInstall(&t3_tcb);
		delay_ms(100);
		usartWriteLine(USART,"DELETE TASK 2\n");
		ROSA_tcbDelete(&t2_tcb);
	}
}

/*************************************************************
 * Task3
 ************************************************************/
void task3(void)
{
	while(1) {
		usartWriteLine(USART,"EXECUTE TASK 3\n");
		delay_ms(100);
		usartWriteLine(USART,"DELETE TASK 3\n");
		ROSA_tcbDelete(&t3_tcb);
	}
}


/*************************************************************
 * Main function for TESTCASE 1
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	
	//Create Tasks
	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
	ROSA_tcbInstall(&t1_tcb);
	
	
	
	//Start the ROSA kernel
	ROSA_schedulerStart();
	
	/* Execution will never return here */
	while(1);
}