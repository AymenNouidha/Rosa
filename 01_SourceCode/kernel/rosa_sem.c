/*
 * rosa_sem.c
 *
 */ 

#include "kernel/rosa_sem.h"


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
	semaphoreHandle sem;
	return sem;
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
int ROSA_semaphoreGive(semaphoreHandle semaphore)
{
	return 0;
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
int ROSA_semaphoreTake(semaphoreHandle semaphore, int waitTime)
{
	return 0;
} 