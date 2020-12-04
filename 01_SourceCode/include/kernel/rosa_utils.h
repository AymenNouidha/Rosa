/*
 * rosa_utils.h
 *
 */ 


#ifndef ROSA_UTILS_H_
#define ROSA_UTILS_H_

// Extract task from ready queue
void extractTaskFromTCBLIST(void);

// Extract task from waiting queue
void extractTaskFromDELAYLIST(void);

// Insert task to ready queue
void insertTaskToTCBLIST(void);

// Insert task to waiting queue
void insertTaskToDELAYLIST(void);



#endif /* ROSA_UTILS_H_ */