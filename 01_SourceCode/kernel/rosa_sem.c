/*
 * rosa_sem.c
 */ 

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_sem.h"
#include "kernel/rosa_int.h"
#include "kernel/rosa_tim.h"

/***********************************************************
 * ROSA_semaphoreCreate
 *
 * Comment:
 *	Create a semaphore
 * Return : semaphoreHandle
 *			The handle of the newly created semaphore
 **********************************************************/
semaphoreHandle ROSA_semaphoreCreate(int priorityCeiling)
{
	// set variables of the SCB to initial values
	semaphoreHandle sem;
	sem.isFree=true;
	//Set the ceiling of the semaphore (and increase by one caused by idle task prio protection)
	sem.ceilPrio=priorityCeiling+1;
	sem.storedPrio=0;
	sem.nextsem = NULL;
	//return handler to the semaphore
	return sem;
}

/***********************************************************
 * ROSA_semaphoreTake
 *
 * Comment:
 * 	Takes a given semaphore
 * Return : unsigned int
 *         0 - if the semaphore could not be taken
 *		   1 - semaphore is taken
 *        -1 - for errors
 **********************************************************/
int ROSA_semaphoreTake(semaphoreHandle* semaphore, uint32_t waitTime)
{
	interruptDisable();
	// when the semaphore is free take it
	if (semaphore->isFree == true)
	{
		//set the status to taken
		semaphore->isFree = false;
		//store previous priority of the taking Task
		semaphore->storedPrio = EXECTASK->priority;
		//adapt the Task priority (IPCP)
		if (semaphore->ceilPrio > EXECTASK->priority)
			EXECTASK->priority = semaphore->ceilPrio;
		//add sem to the sem list of the task
		semaphore->nextsem = EXECTASK->lastsem;
		EXECTASK->lastsem = semaphore;
		interruptEnable();
		return 1;
	}
	
	// when the semaphore is not free, then try to take again after the waiting time
	else
	{
		interruptEnable();
		if (waitTime == 0)
		{
			return 0;
		}
		else
		{
			//Set the task to the waininglist for the given amount of time
			ROSA_sysTickWait(waitTime);
			return ROSA_semaphoreTake(semaphore, 0);
		}
	}
	return -1;
}

/***********************************************************
 * ROSA_semaphoreGive
 *
 * Comment:
 * 	Releases a given semaphore
 * Return : unsigned int
 *         0 - for errors
 *		   1 - semaphore is released
 **********************************************************/
int ROSA_semaphoreGive(semaphoreHandle* semaphore)
{
	interruptDisable();
	// check whether semaphore was taken
	if (semaphore->isFree == false)
	{
		//Reset the priority of the task
		EXECTASK->priority = semaphore->storedPrio;
		//set state of sem to free
		semaphore->isFree = true;
		//Error exception to protect system from not well nested semaphores
		//If this fails the tasks were released in the wrong order!
		if(semaphore != EXECTASK->lastsem)
		{
			
			while(1);
		}
		//remove sem from semaphore List
		EXECTASK->lastsem = semaphore->nextsem;
		semaphore->nextsem = NULL;
		interruptEnable();
		return 1;
	}
	
	interruptEnable();
	return 0;
}
