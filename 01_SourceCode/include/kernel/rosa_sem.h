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

struct semaphore_handle
{
    bool isFree; //False if semaphore is locked. Initialize to true.
    int ceilPrio; //Initialize to semaphore's highest prio task.
    int storedPrio; //Stores the priority the locking task has now.
};

//Creates a semaphore
semaphoreHandle ROSA_semaphoreCreate(int priorityCeiling);

//Takes a semaphore
int ROSA_semaphoreTake(semaphoreHandle semaphore, uint_32 waitTime);

//Releases a semaphore
int ROSA_semaphoreGive(semaphoreHandle semaphore);

#endif /* ROSA_SEM_H_ */
