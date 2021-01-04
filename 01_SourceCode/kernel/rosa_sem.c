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
semaphore_handle ROSA_semaphoreCreate(int priorityCeiling)
{
	semaphore_handle sem ;
	sem.isFree=true;
	sem.ceilPrio=priorityCeiling+1;
	sem.storedPrio=0;
	sem.nextsem=NULL;
	//= {.isFree = true; .ceilPrio = priorityCeiling;.storedPrio=0};
	return sem;
}

//TODO: Replace runningPrio with whatever the actual name is!

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
int ROSA_semaphoreTake(semaphore_handle* semaphore, uint32_t waitTime)
{
	interruptDisable();
	if (semaphore->isFree == true)
	{
		semaphore->isFree = false;
		semaphore->nextsem = EXECTASK->lastsem;
		EXECTASK->lastsem = semaphore;
		semaphore->storedPrio = EXECTASK->priority;
		if (semaphore->ceilPrio > EXECTASK->priority)
			EXECTASK->priority = semaphore->ceilPrio;
		interruptEnable();
		return 1;
	}
	
	else
	{
		interruptEnable();
		if (waitTime == 0)
		{
			return 0;
		}
		else
		{
			ROSA_sysTickWait(waitTime);
			return ROSA_semaphoreTake(semaphore, 0);
		}
		//interruptEnable();
	}
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
int ROSA_semaphoreGive(semaphore_handle* semaphore)
{
	interruptDisable();
	if (semaphore->isFree == false)
	{
		EXECTASK->priority = semaphore->storedPrio;
		EXECTASK->lastsem = semaphore->nextsem;
		semaphore->nextsem = NULL;
		semaphore->isFree = true;
		interruptEnable();
		return 1;
	}
	
	interruptEnable();
	return 0;
}
