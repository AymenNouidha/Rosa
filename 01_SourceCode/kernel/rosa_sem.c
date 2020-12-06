/*
 * rosa_sem.c
 */ 

//Kernel includes
#include "kernel/rosa_ker.h"
#include "kernel/rosa_sem.h"

/***********************************************************
 * ROSA_semaphoreCreate
 *
 * Comment:
 *	Create a semaphore
 * Return : semaphoreHandle
 *			The handle of the newly created semaphore
 **********************************************************/
semaphoreHandle semaphoreCreate(int priorityCeiling)
{
	semaphoreHandle sem = {.isFree = true; .ceilPrio = priorityCeiling};
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
int semaphoreTake(semaphoreHandle* semaphore, uint_32 waitTime)
{
	interruptDisable(void);
	if (semaphore->isFree == true)
	{
		semaphore->isFree = false;
		semaphore->storedPrio = EXECTASK->runningPrio;
		if (semaphore->ceiling > EXECTASK->runningPrio)
			EXECTASK->runningPrio = semaphore->ceiling;
		interruptEnable(void);
		return 1;
	}
	interruptEnable(void);
	else
	{
		if (waitTime == 0)
		{
			return 0;
		}
		else
		{
			ROSA_sysTickWait(waitTime);
			return ROSA_semaphoreTake(semaphore, 0);
		}
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
int semaphoreGive(semaphoreHandle* semaphore)
{
	interruptDisable(void);
    if (semphor->isFree == false)
    {
		EXECTASK->runningPrio = semaphore->storedPrio;
        semaphore->isFree = true;
        interruptEnable(void);
        return 1;
    }
    interruptEnable(void);
    else
        return 0;
}
