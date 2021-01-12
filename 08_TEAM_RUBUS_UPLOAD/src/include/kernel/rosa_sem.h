/*
 * rosa_sem.h
 */ 

#ifndef ROSA_SEM_H_
#define ROSA_SEM_H_
#include <stdint.h>
#include <stdbool.h>

/**********************************************************************************************
 * semaphoreHandle
 *
 * Description: the semaphore struct used by all semaphores
 * Elements: 
 *           isFree: false if semaphore is locked, true otherwise. Initializes to true.
 *           ceilPrio: used for IPCP. Initializes to highest priority task using the semaphore.
 *			 storedPrio: Stores the priority the locking task has before taking the sem.
 *			 nextsem: semaphore List for all semaphores taken by a task
 ***********************************************************************************************/

typedef struct semaphoreHandle
{
    bool isFree; //False if semaphore is locked. Initialize to true.
    int ceilPrio; //Initialize to semaphore's highest prio task.
    int storedPrio; //Stores the priority the locking task has now.
	struct semaphoreHandle* nextsem; //Pointer to the next semaphore taken by a task
}semaphoreHandle;

//Creates a semaphore
semaphoreHandle ROSA_semaphoreCreate(int priorityCeiling);

//Takes a semaphore
int ROSA_semaphoreTake(semaphoreHandle* semaphore, uint32_t waitTime);

//Releases a semaphore
int ROSA_semaphoreGive(semaphoreHandle* semaphore);

#endif /* ROSA_SEM_H_ */