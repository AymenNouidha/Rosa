/*
 * rosa_sem.h
 */ 


#ifndef ROSA_SEM_H_
#define ROSA_SEM_H_

#include <stdbool.h>
/**********************************************************************************************
 * semaphoreHandle
 *
 * Description: the semaphore struct used by all semaphores
 * Elements: 
 *           isFree: false if semaphore is locked, true otherwise. Initializes to true.
 *           ceilPrio: used for IPCP. Initializes to highest priority task using the semaphore.
 ***********************************************************************************************/

typedef struct{
	bool isFree; 
	int ceilPrio;
}semaphoreHandle;

//Creates a semaphore
semaphoreHandle ROSA_semaphoreCreate(int priorityCeiling);

//Releases a semaphore
int ROSA_semaphoreGive(semaphoreHandle semaphore);

//Takes a semaphore
int ROSA_semaphoreTake(semaphoreHandle semaphore, int waitTime);

#endif /* ROSA_SEM_H_ */