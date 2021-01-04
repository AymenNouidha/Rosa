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
 ***********************************************************************************************/

typedef struct semaphore_handle
{
    bool isFree; //False if semaphore is locked. Initialize to true.
    int ceilPrio; //Initialize to semaphore's highest prio task.
    int storedPrio; //Stores the priority the locking task has now.
    struct semaphore_handle* nextsem;
}semaphore_handle;

//Creates a semaphore
semaphore_handle ROSA_semaphoreCreate(int priorityCeiling);

//Takes a semaphore
int ROSA_semaphoreTake(semaphore_handle* semaphore, uint32_t waitTime);

//Releases a semaphore
int ROSA_semaphoreGive(semaphore_handle* semaphore);

#endif /* ROSA_SEM_H_ */
