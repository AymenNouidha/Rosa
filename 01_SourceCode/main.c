/*###################################################################################
*					TEAM RUBUS TESTCASE 1
*						Original
* ###################################################################################
*/
/****************************
/ Testcase 1: SEMAPHORES
/	Right implementation: Task 2 should never execute (The second LED should not turn on)
/ 	Wrong implementation: Task 2 is executing (second LED is turning on and statement is printed to usart)
/	Problem: wrong implementation of priorities of IPCP
*/
/****************************
*/


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







// /*###################################################################################
// *					TEAM RUBUS TESTCASE 2
// *						Adapted
// * ###################################################################################
// */
// 
// 
// /****************************
// / Testcase 2: SEMAPHORES AND DELAY IN SEM TAKE
// /	Right Implementation: LED0 goes on, Led1 goes on, Led2 goes on, led0 goes off, led1 goes of and then starts blinking  
// /	Wrong Implementation: LED2 will never go on 
// */
// /****************************
// */
// /* Tab size: 4 */
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// #include "drivers/usart.h"
// 
// //Include configuration
// #include "rosa_config.h"
// #include "kernel/rosa_sem.h"
// #include "kernel/rosa_tim.h"
// #include "kernel/rosa_scheduler.h"
// 
// 
// //Data block for the semaphores
// semaphoreHandle sem1;
// 
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// static tcb t2_tcb;
// 
// #define T3_STACK_SIZE 0x40
// static int t3_stack[T3_STACK_SIZE];
// static tcb t3_tcb;
// 
// /*************************************************************
//  * Task1
//  * 
//  * 
//  ************************************************************/
// void task1(void)
// {
// 	while(1) {
// 		ledOn(LED0_GPIO);
// 		
// 		//Protect the code to just execute when semaphore is taken
// 		if(ROSA_semaphoreTake(&sem1,0))
// 		{
// 			ROSA_tcbInstall(&t2_tcb);
// 			ROSA_tcbInstall(&t3_tcb);
// 			delay_ms(150);
// 			ROSA_sysTickWait(3000);
// 			//changed to switch the led off before giving the semaphore
// 			ledOff(LED0_GPIO);
// 			ROSA_semaphoreGive(&sem1);
// 			// delay added
// 			delay_ms(500);
// 		}	
// 	}
// }
// 
// 
// /*************************************************************
//  * Task2
//  ************************************************************/
// void task2(void)
// {
// 	while(1) {
// 		ledOn(LED1_GPIO);
// 		//Protect the code to just execute when semaphore is taken
// 		if(ROSA_semaphoreTake(&sem1,3000))
// 		{
// 			delay_ms(150);
// 			ROSA_semaphoreGive(&sem1);
// 			ledOff(LED1_GPIO);
// 			// delay added
// 			delay_ms(150);
// 		}
// 	}
// }
// /*************************************************************
//  * Task3
//  ************************************************************/
// void task3(void)
// {
// 	while(1) {
// 		ledOn(LED2_GPIO);
// 	}
// }
// 
// 
// /*************************************************************
//  * Main function for TESTCASE 2
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 	
// 	// Create needed Semaphores
// 	sem1=ROSA_semaphoreCreate(6);
// 	
// 	//Create Tasks
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	
// 	ROSA_tcbCreate(&t2_tcb, "tsk2", task2,6, t2_stack, T2_STACK_SIZE);
// 	ROSA_tcbCreate(&t3_tcb, "tsk3", task3,4, t3_stack, T3_STACK_SIZE);
// 	
// 	
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	
// 	/* Execution will never return here */
// 	while(1);
// }





// /*###################################################################################
// *					TEAM RUBUS TESTCASE 3
// *						Adapted
// * ###################################################################################
// */
// /****************************
// / Testcase 3: SCHEDULER INTERRUPT AND DYNAMIC CREATION AND DELETION OF TASKS
// / Right implementation: USART output: EXECUTE TASK 1; EXECUTE TASK 2; EXECUTE TASK 3; DELETE TASK 3; DELETE TASK 2; DELETE TASK 1;
// /
// */
// /****************************
// */
// 
// 
// 
// /* Tab size: 4 */
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// #include "drivers/usart.h"
// 
// //Include configuration
// #include "rosa_config.h"
// #include "kernel/rosa_sem.h"
// #include "kernel/rosa_tim.h"
// #include "kernel/rosa_scheduler.h"
// 
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// static tcb t2_tcb;
// 
// #define T3_STACK_SIZE 0x40
// static int t3_stack[T3_STACK_SIZE];
// static tcb t3_tcb;
// 
// //NEW LINES ADDED IN HERE
// void task1();
// void task2();
// void task3();
// 
// /*************************************************************
//  * task1
//  * 
//  * 
//  ************************************************************/
// void task1(void)
// {
// 	while(1) {
// 		
// 		usartWriteLine(USART,"EXECUTE TASK 1\n");
// 		ROSA_tcbCreate(&t2_tcb, "tsk2", task2,3, t2_stack, T2_STACK_SIZE);
// 		ROSA_tcbInstall(&t2_tcb);
// 		delay_ms(100);
// 		usartWriteLine(USART,"DELETE TASK 1\n");
// 		ROSA_tcbDelete(&t1_tcb);
// 	}
// }
// 
// /*************************************************************
//  * Task2
//  ************************************************************/
// void task2(void)
// {
// 	while(1) {
// 		
// 		usartWriteLine(USART,"EXECUTE TASK 2\n");
// 		ROSA_tcbCreate(&t3_tcb, "tsk3", task3,6, t3_stack, T3_STACK_SIZE);
// 		ROSA_tcbInstall(&t3_tcb);
// 		delay_ms(100);
// 		usartWriteLine(USART,"DELETE TASK 2\n");
// 		ROSA_tcbDelete(&t2_tcb);
// 	}
// }
// 
// /*************************************************************
//  * Task3
//  ************************************************************/
// void task3(void)
// {
// 	while(1) {
// 		usartWriteLine(USART,"EXECUTE TASK 3\n");
// 		delay_ms(100);
// 		usartWriteLine(USART,"DELETE TASK 3\n");
// 		ROSA_tcbDelete(&t3_tcb);
// 	}
// }
// 
// 
// /*************************************************************
//  * Main function for TESTCASE 1
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 	
// 	
// 	//Create Tasks
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1,1, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	
// 	
// 	
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	
// 	/* Execution will never return here */
// 	while(1);
// }
// 





// /*###################################################################################
// *					TEAM OSE TESTCASE 1
// *						Adapted
// * ###################################################################################
// */
// 
// /*****************************************************************************
// TEST CASE: Usage of non existing semaphore.
// A task tries to take a unitialized semaphore handled which has not been created through the ROSA_semaphoreCreate function.
// *****************************************************************************/
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// #include "kernel/rosa_sem.h"
// 
// //Driver includes
// #include "drivers/led.h"
// 
// //Include configuration
// #include "rosa_config.h"
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// // Create uninitialized semaphore handle
// semaphoreHandle *sem=NULL;
// 
// /*************************************************************
//  * Task1
//  * LED0: The test failed due to the semaphore being taken when it shouldn't. 
//  * LED1: The test failed due to the semaphore take attempt, returned 0 (indicating that it couldn't 
//  * be taken due to it being locked), when it should have returned -1 instead.
//  * LED2: The test passed since the ROSA_semaphoreTake function returned -1.
//  ************************************************************/
// void task1(void)
// {
// 	// Try to take the semaphore and store the result in a variable.
// 	int result = ROSA_semaphoreTake(sem, 0);
// 	
// 	// We took the semaphore
// 	if (result == 1)
// 	{
// 		// Light LED 0 to indicate that we took the semaphore,
// 		// if this happens the test has failed.
// 		ledOn(LED0_GPIO);
// 	}
// 	// We could not take the semaphore due to it being locked
// 	else if (result == 0)
// 	{
// 		// Light LED 1 to indicate that we couldnt take the semaphore due to it being locked,
// 		// if this happens the test has failed.
// 		ledOn(LED1_GPIO);
// 	}
// 	// We could not take the semaphore due to an error.
// 	else if (result == -1)
// 	{
// 		// This is the expected outcome of this test, so if this branch is reached,
// 		// light LED 2 to indicate that the test has passed.
// 		ledOn(LED2_GPIO);
// 	}
// 
// 	// Infinite loop so the tasks doesn't return.
// 	while (1)
// 	{
// 	}
// }
// 
// /*************************************************************
//  * Main function
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 
// 	//Create tasks and install them into the ROSA kernel
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, 2, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	/* Execution will never return here */
// 	while (1);
// }


// /*###################################################################################
// *					TEAM OSE TESTCASE 2
// *						Adapted
// * ###################################################################################
// */
// 
// /*****************************************************************************
// TEST CASE: Multiple instances of same task handle.
// Create two tasks using the same task handle. The second task creation should not 
// work since the task handle is already in use.
// *****************************************************************************/
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// 
// //Include configuration
// #include "rosa_config.h"
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// 
// /*************************************************************
//  * Task1 - Dummy task that we can pass into the ROSA_tcbCreate function
//  ************************************************************/
// void task1(void)
// {
// 	while (1);
// }
// 
// /*************************************************************
//  * Main function
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 
// 	//Create a task and install it into the ROSA kernel
// 	int returnValue = ROSA_tcbCreate(&t1_tcb, "tsk1", task1, 2, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	// The return value should be 1, since the tcb handle should be empty and the task
// 	// shouldn't have any problems being created.
// 	if (returnValue == 1)
// 	{
// 		// The first step of the test has passed
// 		ledOn(LED0_GPIO);
// 	}
// 
// 	// Create another task using the same tcb handle
// 	returnValue = ROSA_tcbCreate(&t1_tcb, "tsk2", task1, 2, t2_stack, T2_STACK_SIZE);
// 	
// 	// This time the return value should be 3, which means that the tcb handle was already used for another task.
// 	if (returnValue == 3)
// 	{
// 		// Light the second LED to indicate that the second test passed.
// 		ledOn(LED1_GPIO);
// 	}
// 
// 	while (1);
// }


// /*###################################################################################
// *					TEAM OSE TESTCASE 3
// *						Adapted
// * ###################################################################################
// /*****************************************************************************
// TEST CASE: Creation and deletion of tasks during run-time.
// Create a task that blinks led 0. After 5 iterations of the while loop, a second
// task is created which blinks led 1. After 10 iterations the second task is deleted
// and led 1 is turned off. Now, if led 0 is blinking and led 1 is turned off, the 
// system successfully created and deleted a task during run-time and the test is passed.
// But if led 0 and 1 are still blinking, the system did not manage to delete the second
// task so the test had failed.
// *****************************************************************************/
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// 
// //Include configuration
// #include "rosa_config.h"
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// static tcb t2_tcb;
// 
// void task2();
// /*************************************************************
//  * Task1
//  * LED0 will be blinking as long as this task is running. After 3.5 seconds, this task will create 
//  * another task that blinks LED 1. If both LED 0 and LED 1 are blinking for while, 
//  * and then LED 1 is turned off while LED0 keeps blinking, the test has passed.
//  ************************************************************/
// void task1(void)
// {
// 	int iteration = 0;
// 	while (1)
// 	{
// 		// Blink LED 0 to indicate that task1 is running
// 		ledOn(LED0_GPIO);
// 		delay_ms(350);
// 		
// 		ledOff(LED0_GPIO);
// 		delay_ms(350);
// 		
// 		// After the 5h iteration, create a new task
// 		if (iteration == 5)
// 		{
// 			ROSA_tcbCreate(&t2_tcb, "tsk2", task2, 2, t2_stack, T2_STACK_SIZE);
// 		}
// 
// 		// After the 10th iteration, delete task2
// 		if (iteration == 10)
// 		{
// 			ROSA_tcbDelete(&t2_tcb);
// 			
// 			// ... and turn of the LED that task2 were blinking.
// 			ledOff(LED1_GPIO);
// 			
// 			// If LED1 stays off and LED0 keeps blinking, task2 has been deleted and is no longer scheduled,
// 			// which means that the test has passed.
// 		}
// 
// 		++iteration;
// 		
// 	}
// }
// 
// /*************************************************************
//  * Task2
//  * LED2 will be binking as long as this task is running.
//  ************************************************************/
// void task2(void)
// {
// 	while (1)
// 	{
// 		// Blink LED 1 to indicate that task2 is running, which was created from task1
// 		ledOn(LED1_GPIO);
// 		delay_ms(350);
// 		
// 		ledOff(LED1_GPIO);
// 		delay_ms(350);
// 	}
// }
// 
// /*************************************************************
//  * Main function
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 
// 	//Create tasks and install them into the ROSA kernel
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, 2, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	
// 	/* Execution will never return here */
// 	while (1);
// }


// /*###################################################################################
// *					TEAM VxWorks TESTCASE 1
// *						Adapted
// * ###################################################################################
// */
// /*****************************************************************************
// *	Test case 1
// *	Group: VxWorks
// *****************************************************************************/
// 
// /* Tab size: 4 */
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// #include "kernel/rosa_sem.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// #include "drivers/usart.h"
// 
// //Include configuration
// #include "rosa_config.h"
// 
// //Data blocks for the tasks
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// static tcb t2_tcb;
// 
// #define T3_STACK_SIZE 0x40
// static int t3_stack[T3_STACK_SIZE];
// static tcb t3_tcb;
// 
// semaphoreHandle sem;
// 
// /*************************************************************
//  * Task1
//  * LED3 lights up
//  ************************************************************/
// void task1(void)
// {
// 	while(1) {
// 	    //Atemps to take semaphore
// 	    ROSA_semaphoreTake(&sem, 9999);
// 		ledOn(LED1_GPIO);
// 		while(1);
// 		ROSA_semaphoreGive(&sem);
// 	}
// }
// 
// /*************************************************************
//  * Task2
//  * LED2 lights up
//  ************************************************************/
// void task2(void)
// {
// 	while(1) {
// 		//Atemps to take semaphore
// 	    ROSA_semaphoreTake(&sem, 9999);
// 		ledOn(LED2_GPIO);
// 		while(1);
// 		ROSA_semaphoreGive(&sem);
// 	}
// }
// 
// /*************************************************************
//  * Task3
//  * LED3 lights up
//  * Takes semaphore, do a delay, gives semaphore
//  * LED3 goes dark
//  ************************************************************/
// void task3(void)
// {
// 	while(1) {
// 		//Atemps to take semaphore
// 	    ROSA_semaphoreTake(&sem, 9999);
// 		ledOn(LED3_GPIO);
// 		ROSA_sysTickWait(1000);
// 		ROSA_semaphoreGive(&sem);
// 		
// 		ROSA_tcbDelete(&t2_tcb);
//         ROSA_yield();
// 	}
// }
// 
// /*************************************************************
//  * Main function
//  ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 	sem = ROSA_semaphoreCreate(3);
// 
// 	//Create tasks and install them into the ROSA kernel
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", task1, 1, t1_stack, T1_STACK_SIZE);
//     ROSA_tcbInstall(&t1_tcb);
//     ROSA_tcbCreate(&t2_tcb, "tsk2", task2, 2, t2_stack, T2_STACK_SIZE);
//     ROSA_tcbInstall(&t2_tcb);
//     ROSA_tcbCreate(&t3_tcb, "tsk3", task3, 3, t3_stack, T3_STACK_SIZE);
//     ROSA_tcbInstall(&t3_tcb);
// 	
// 
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	/* Execution will never return here */
// 	while(1);
// }
// 	


// /*###################################################################################
// *					TEAM VxWorks TESTCASE 2
// *						Adapted
// * ###################################################################################
// */
// 
// //Standard library includes
// #include <avr32/io.h>
// 
// //Kernel includes
// #include "kernel/rosa_ker.h"
// #include "kernel/rosa_sem.h"
// 
// //Driver includes
// #include "drivers/led.h"
// #include "drivers/delay.h"
// #include "drivers/usart.h"
// 
// //Include configuration
// #include "rosa_config.h"
// 
// /*****************************************************************************
// *	Test case 2
// *	Group: VxWorks
// *****************************************************************************/
// 
// #define T1_STACK_SIZE 0x40
// static int t1_stack[T1_STACK_SIZE];
// static tcb t1_tcb;
// 
// #define T2_STACK_SIZE 0x40
// static int t2_stack[T2_STACK_SIZE];
// static tcb t2_tcb;
// semaphoreHandle s1 ;
// 
// /*************************************************************
// * Task1
// * LED1 lights up
// ************************************************************/
// void T1(void)
// {
// 	while(1) {
// 		ROSA_semaphoreTake(&s1,1000);
// 		ledOn(LED1_GPIO);
// 		ROSA_tcbDelete(&t1_tcb);
// 		ROSA_semaphoreGive(&s1);
// 	}
// }
// 
// /*************************************************************
// * Task2
// * LED2 lights up
// * LED1 goes dark
// ************************************************************/
// void T2(void)
// {
// 	while(1) {
// 		ROSA_semaphoreTake(&s1,2000);
// 		ledOn(LED2_GPIO);
// 		ledOff(LED1_GPIO);
// 		ROSA_semaphoreGive(&s1);
// 	}
// }
// /*************************************************************
// * Main function
// ************************************************************/
// int main(void)
// {
// 	//Initialize the ROSA kernel
// 	ROSA_init();
// 	//Create tasks and install them into the ROSA kernel
// 	ROSA_tcbCreate(&t1_tcb, "tsk1", T1, 2, t1_stack, T1_STACK_SIZE);
// 	ROSA_tcbInstall(&t1_tcb);
// 	ROSA_tcbCreate(&t2_tcb, "tsk2", T2, 1, t2_stack, T2_STACK_SIZE);
// 	ROSA_tcbInstall(&t2_tcb);
// 	
// 	s1 = ROSA_semaphoreCreate(2);
// 	//Start the ROSA kernel
// 	ROSA_schedulerStart();
// 	
// 	while(1);
// }





// /*###################################################################################
// *					TEAM VxWorks TESTCASE 3
// *						Adapted
// * ###################################################################################
// */
// This testcase is not regarding the API and so our system would not pass that, as our implementation is different.
